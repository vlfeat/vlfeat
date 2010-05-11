#!/usr/bin/python
# file:        formatter.py
# author:      Andrea Vedaldi
# description: Utility to format MATLAB comments.

# AUTORIGHTS
# Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
#
# This file is part of VLFeat, available in the terms of the GNU
# General Public License version 2.

"""
MDOC fromats the help block of a MATLAB M-file based on a simple set
of rules. Pharagraphs, verbatim sections, lists and other structures
are automatically instantiated by looking at blank lines, indentation
and a few decoration symbols.

The documentation starts at a conventional indentation level N (by
default 2). A block of non-epmty lines prefixed by N characters is
considered a paragraph. For instance

 |  Bla bla bla
 |  bla bla bla.
 |
 |  Bla bla.

generates two pharagraps. If there are more than N white spaces,
then the block is taken verbatim instead (and rendered in <pre> HTML
tags). For instance

 |  Bla bla bla
 |   Code Code Code
 |
 |   Code Code Code

generates one paragraph followed by one verbatim section.
"""

import xml.dom.minidom
import sys
import os
import re

__mpname__           = 'MDocFormatter'
__version__          = '0.1'
__date__             = '2008-01-01'
__description__      = 'MDoc formatting module'
__long_description__ = __doc__
__license__          = 'GPL'
__author__           = 'Andrea Vedaldi'

# terminal
class Terminal:
    def isa(self, classinfo):
        return isinstance(self, classinfo)

# empty terminal
class E (Terminal):
    pass

# blank line
class B (Terminal):
    content = ""

# non-blank line
class L (Terminal):
    indent  = 0

# regular line
class PL (L):
    pass

# line with bullet
class BL (L):
    bullet = None
    inner_indent = 0

# line with description
class DL (L):
    pass

# --------------------------------------------------------------------
def lex(line):
# --------------------------------------------------------------------
    """
    Parse the string LINE to a terminal symbol. Each line corresponds
    to exactly one terminal type. However, terminal types are the leaf
    of a hierarchy of types.
    """

    match = re.match(r"\s*\n?$", line) ;
    if match: return B()

    match = re.match(r"(\s*)(.*)::(.*)\n?$", line)
    if match:
        x = DL()
        x.indent        = len(match.group(1))
        x.content       = match.group(2)
        x.inner_content = match.group(3)
        return x

    match = re.match(r"(\s*)([-\*#]\s*)(\S.*)\n?$", line)
    if match:
        x = BL()
        x.indent        = len(match.group(1))
        x.inner_content = match.group(3)
        x.bullet        = match.group(2)
        x.inner_indent  = x.indent + len(x.bullet)
        x.content       = x.bullet + x.inner_content
        return x

    match = re.match(r"(\s*)(\S.*)\n?$", line)
    if match:
        x = PL()
        x.indent  = len(match.group(1))
        x.content = match.group(2)
        return x

# --------------------------------------------------------------------
class Lexer(object):
# --------------------------------------------------------------------
    """
    l = Lexer(LINES) parses the array of strings LINES. Lexer has a
    head pointing to the current line. The head can be controlled by
    the following methods:

    l.next() advances the head and fetches the next terminal.
    l.back() moves back the head.
    l.getpos() returns the head position.
    l.seek(POS) sets the head position to POS.
    """
    def __init__(self, lines):
        self.tokens = []
        self.pos    = -1
        for line in lines:
            self.tokens.append(lex(line))

    def next(self):
        self.pos = self.pos + 1
        if self.pos >= len(self.tokens):
            return E()
        else:
            return self.tokens [self.pos]

    def seek(self, pos):
        self.pos = pos

    def back(self):
        if self.pos >=0: self.pos -= 1

    def rewrite(self, str):
        self.tokens [self.pos] = str ;

    def getpos(self):
        return self.pos

    def __str__(self):
        str = ""
        for i,t in enumerate(self.tokens):
             str += "%5d) %s %s\n" % (i, t.__class__.__name__,t.content)
        return str

# --------------------------------------------------------------------
class Formatter:
# --------------------------------------------------------------------
    """
    f = Formatter(LINES) parse the array of strings LINES.

    f = Formatter(LINES, FUNCS) takes the dictionary of functions
    FUNCS. Function names must be uppercase. The dictionary entries
    are used to cross link functions in the generated documentation.

    Formatter(LINES, FUNCS, LINKTYPE) produces links of the specified
    type.  Use 'a' for HTML anchors and 'wiki' for MediaWiki style
    links.

    f.toDOM() process the data to construct an XML (HTML) representation
    of them.
    """
    def __init__ (self, lines, funcs={}, linktype='a'):
        self.indentinit = 0
        lineone = lines[0]
        while lineone.startswith(' '):
            lineone = lineone[1:]
            self.indentinit += 1

        self.tokens = Lexer(lines)
        self.xmldoc = xml.dom.minidom.Document()
        self.funcs  = funcs
        self.linktype = linktype
        #print self.tokens

    def toTextNode(self,s):
        return self.xmldoc.createTextNode(unicode(s, 'iso-8859-1'))

    def addAttr(self, tag, attr, val):
        x = self.xmldoc.createAttribute(attr)
        x.nodeValue = val
        tag.setAttributeNode(x)

    def addText(self, tag, s):
        txt = self.toTextNode(s)
        tag.appendChild(txt)

    def addFancyText(self, tag, s):
        "Adds text while transforming function references to links."
        xs = []
        iter = re.finditer('([A-Z][A-Z0-9_]*)\([^\)]*\)', s)
        last = -1

        for i in iter:
            func_name = i.group(1)

            # lookup function name in dictionary
            if self.funcs.has_key(func_name):
                # retrieve function HTML location
                func_href = self.funcs[func_name]

                # add text so far
                xs.append(self.toTextNode(s[last+1:i.start()]))

                if self.linktype == 'a':
                    # add link to function
                    atag = self.xmldoc.createElement(u"a")
                    self.addText(atag, i.group(1))
                    atag.setAttribute(u"href", u"%s" % (func_href))
                    xs.append(atag)
                elif self.linktype == 'wiki':
                    linktxt = "[[%s|%s]]" % (func_href, i.group(1))
                    xs.append(self.toTextNode(linktxt))

                # set head
                last = i.start()+len(i.group(1))-1
            #else:
            #    print "function: %s not found" % func_name

        xs.append(self.toTextNode(s[last+1:]))
        for x in xs:
            tag.appendChild(x)

    # ................................................................
    # E, B, L, PL, BL, DL, ...
    def parse_Terminal(self, T):
        "If the next terminal on the stream is of type T, the terminal"
        "is extracted and returned. Otherwise the function returns None"
        pos = self.tokens.getpos()
        t = self.tokens.next()
        if t.isa(T):
            return t
        self.tokens.seek(pos)
        return None

    # ................................................................
    # DIV(N) -> (B | P(N) | BL(N) | DL(N) | V(N))+
    def parse_DIV(self, indent):
        "Parse a DIV(N) symbol. A DIV(N) a sequence of blank"
        "lines (B or other blocks at indentation level N, such as"
        "pharagraphs P(N), bullet lists BL(N), description lists DN(N)"
        pos = self.tokens.getpos()
        xs = []
        while True:
            x = self.parse_Terminal(B)
            if x: continue

            x = self.parse_P(indent)
            if x:
                xs.append(x)
                continue

            x = self.parse_V(indent)
            if x:
                xs.append(x)
                continue

            x = self.parse_UL(indent)
            if x:
                xs.append(x)
                continue

            x = self.parse_DL(indent)
            if x:
                xs.append(x)
                continue

            break
        if len(xs) == 0: return None
        return xs

    # ................................................................
    # P(N) -> PL(N) L(N)*
    def parse_P(self, indent):
        content = "\n"
        good = False
        pos = self.tokens.getpos()

        # Introduced by PL
        x = self.parse_Terminal(PL)
        if x:
            if x.indent == indent:
                content += x.content + "\n"
                good = True
            else:
                self.tokens.back()
        if not good:
            return None

        # Continued by zero or more L
        while True:
            x = self.parse_Terminal(L)
            if x:
                if x.indent == indent:
                    content += x.content + "\n"
                    good = True
                    continue
                else:
                    self.tokens.back()
            break

        ptag = self.xmldoc.createElement("p")
        self.addFancyText(ptag, content)
        return ptag

    # ................................................................
    # V(N) -> L(M)+, M > N
    def parse_V(self, indent):
        content = "\n"
        good = False
        pos = self.tokens.getpos()
        while True:
            x = self.parse_Terminal(L)
            if x:
                if x.indent > indent:
                    content += " "*(x.indent - indent) + x.content + "\n"
                    good = True
                    continue
                else:
                    self.tokens.back()
            x = self.parse_Terminal(B)
            if x:
                content += "\n"
                continue
            break
        if good:
            ptag = self.xmldoc.createElement("pre")
            # remove potential blank line at the end
            if content[-2:] == "\n\n":
                content= content[:-1]
            self.addText(ptag, content)
            return ptag
        self.tokens.seek(pos)
        return None

    # ................................................................
    # UL(N) -> ULI(N)+
    def parse_UL(self, indent):
        xs = []
        while True:
            x = self.parse_ULI(indent)
            if x:
                xs.append(x)
                continue
            break
        if len(xs) == 0: return None
        ultag = self.xmldoc.createElement("ul")
        for x in xs:
            ultag.appendChild(x)
        return ultag

    # ................................................................
    # ULI(N) -> UL(N,M) L(M)* DIV(M), M > N
    def parse_ULI(self, indent):
        content = "\n"
        good = False
        pos = self.tokens.getpos()

        # Introduced by UL
        x = self.parse_Terminal(BL)
        if x:
            if x.indent == indent:
                content += x.inner_content + "\n"
                indent   = x.inner_indent
                good = True
            else:
                self.tokens.back()
        if not good:
            return None

        # Continued by zero or more L
        while True:
            x = self.parse_Terminal(L)
            if x:
                if x.indent == indent:
                    content += x.content + "\n"
                    good = True
                    continue
                else:
                    self.tokens.back()
            break
        litag = self.xmldoc.createElement(u"li")
        ptag  = self.xmldoc.createElement(u"p")
        self.addFancyText(ptag, content)
        litag.appendChild(ptag)

        # Continued by DIV
        xs = self.parse_DIV(indent)
        if xs:
            for x in xs:
                litag.appendChild(x)

        return litag


    # ................................................................
    # DL(N) -> DI(N)+
    def parse_DL(self, indent):
        xs = []
        while True:
            x = self.parse_DI(indent)
            if x:
                xs += x
                continue
            break
        if len(xs) == 0: return None
        dltag = self.xmldoc.createElement(u"dl")
        for x in xs:
            dltag.appendChild(x)
        return dltag

    # ................................................................
    # DI(N) -> DL(N) DIV(M)?, M > N
    def parse_DI(self, indent):
        content = "\n"
        good   = False
        pos    = self.tokens.getpos()
        xs     = []

        # Introduced by DL
        x = self.parse_Terminal(DL)
        if x:
            if x.indent == indent:
                content += x.content + "\n"
                good = True
            else:
                self.tokens.back()
        if not good:
            return None

        if False:
            # adds text after :: as part of the description dd
            dttag = self.xmldoc.createElement(u"dt")
            dttxt = self.toTextNode(content)
            dttag.appendChild(dttxt)
            xs.append(dttag)

            # Inject inner_content
            c = x.inner_content.strip()
            if len(c) > 0:
                tk = PL()
                tk.content = x.inner_content
                t = self.tokens.next()
                self.tokens.back()
                if t.isa(L) and t.indent > indent:
                    tk.indent = t.indent
                else:
                    tk.indent = indent+1 ;
                    self.tokens.rewrite(tk)
                    self.tokens.back()
        else:
            # adds text after :: as part of the description term dt
            dttag = self.xmldoc.createElement(u"dt")
            dttxt = self.toTextNode(content)
            dttag.appendChild(dttxt)
            c = x.inner_content.strip()
            if len(c) > 0:
                deftag = self.xmldoc.createElement(u"span")
                self.addAttr(deftag, "class", "defaults")
                self.addText(deftag, c)
                dttag.appendChild(deftag)
            xs.append(dttag)

        # Continued by DIV
        t = self.tokens.next()
        self.tokens.back()
        if t.isa(L) and t.indent > indent:
            xs_ = self.parse_DIV(t.indent)
            if len(xs_) > 0:
                ddtag = self.xmldoc.createElement(u"dd")
                for x in xs_:
                    ddtag.appendChild(x)
                xs.append(ddtag)

        return xs

    # ................................................................
    def toDOM(self):
        # write <mfile></mfile>
        xmf = self.xmldoc.createElement("div")
        xmf.setAttribute(u"class", u"documentation")

        self.xmldoc.appendChild(xmf)

        # parse documentation
        xs = self.parse_DIV(self.indentinit)
        for x in xs: xmf.appendChild(x)

        return self.xmldoc


if __name__ == '__main__':
    text="""
 Lorem Ipsum is simply dummy text of the printing and typesetting
 industry. Lorem Ipsum has been the industry's standard dummy text
 ever since the 1500s, when an unknown printer took a galley of type
 and scrambled it to make a type specimen book. It has survived not
 only five centuries, but also the leap into electronic typesetting,
 remaining essentially unchanged. It was popularised in the 1960s with
 the release of Letraset sheets containing Lorem Ipsum passages, and
 more recently with desktop publishing software like Aldus PageMaker
 including versions of Lorem Ipsum.

 These are links BL(), BL(A,B) and BLA(A,A) (as long as the dictionary
 cites them).

 Mimamama
   verbatim1
   verbatim2
   verbatim3

   verbatim4
   verbatim5
 Lorem Ipsum is simply dummy text of the printing and typesetting
 industry. Lorem Ipsum has been the industry's standard dummy text
 ever since the 1500s, when an unknown printer took a galley of type
 and scrambled it to make a type specimen book. It has survived not
 only five centuries, but also the leap into electronic typesetting,
 remaining essentially unchanged. It was popularised in the 1960s with
 the release of Letraset sheets containing Lorem Ipsum passages, and
 more recently with desktop publishing software like Aldus PageMaker
 including versions of Lorem Ipsum.

 - outer1 /
   outer1 line 2 /
   outer1 line 3 /

   outer1 new paragarph

   - inner1
   - inner2
   - inner3
     continued on next line
       continued with verbatim

       more verbatim after blank
   - inner4
 - outer again
 - outer
 bla

 - list2
 - list4
 - BL()
 - BL(A,B)

 Test descrition::
     Lorem Ipsum is simply dummy text of the printing
     and typesetting industry. Lorem Ipsum has been the industry's
     standard dummy text ever since the 1500s, when an unknown printer
     took a galley of type and scrambled it to make a type specimen
     book. It has survived not only five centuries, but also the leap
     into electronic typesetting, remaining essentially unchanged. It
     was popularised in the 1960s with the release of Letraset sheets
     containing Lorem Ipsum passages, and more recently with desktop
     publishing software like Aldus PageMaker including versions of
     Lorem Ipsum.

 Ancora::
     Bli bli bli
     Blu blu blu

     - list
     - lust
     - last

     Bli bla

  Verbatimmo
"""
    lines = text.splitlines()
    formatter = Formatter(lines, {'BL':'http://www.google.com'}, 'a')
    print formatter.toDOM().toxml("UTF-8")
