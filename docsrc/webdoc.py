#!/usr/bin/python
# file: webdoc.py
# author: Andrea Vedaldi
# description: A website formatter utility

# Copyright (C) 2007-13 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

import cProfile
import types
import xml.sax
import xml.sax.saxutils
import re
import os
import sys
import random
import copy
import htmlentitydefs

from xml.sax.handler import ContentHandler
from xml.sax         import parse
from urlparse        import urlparse
from urlparse        import urlunparse
from optparse        import OptionParser
from doxytag         import Doxytag

# this is used for syntax highlighting
try:
    import pygments
    import pygments.lexers
    import pygments.formatters
    has_pygments = True
except ImportError:
    has_pygments = False

DOCTYPE_XHTML_TRANSITIONAL = \
    '<!DOCTYPE html PUBLIC ' \
    '"-//W3C//DTD XHTML 1.0 Transitional//EN" ' \
    '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">'

# Create a dictonary that maps unicode characters to HTML entities
mapUnicodeToHtmlEntity = { }
for k, v in htmlentitydefs.name2codepoint.items():
    c = unichr(v)
    if c == u'&' or c == u'<' or c == u'>': continue
    mapUnicodeToHtmlEntity [c] = "&%s;"  % k

# This indexes the document nodes by ID
nodeIndex = { }
nodeUniqueCount = 0
doxygenIndex = None
doxygenDir = ''

def getDoxygenURL(tag):
    url = ''
    rootURL = nodeIndex['root'].getPublishURL()
    if rootURL: url += rootURL + '/'
    if doxygenDir: url += doxygenDir + '/'
    url += doxygenIndex.index[tag]
    return url

def getUniqueNodeID(id = None):
    """
    getUniqueNodeID() generates an unique ID for a document node.
    getUniqueNodeID(id) generates an unique ID adding a suffix to id.
    """
    global nodeUniqueCount
    if id is None: id = "id"
    uniqueId = id
    while uniqueId in nodeIndex:
        nodeUniqueCount += 1
        uniqueId = "%s-%d" % (id, nodeUniqueCount)
    return uniqueId

def dumpIndex():
    """
    Dump the node index, for debugging purposes.
    """
    for x in nodeIndex.itervalues():
      print x

def ensureDir(dirName):
    """
    Create the directory DIRNAME if it does not exsits.
    """
    if os.path.isdir(dirName):
        pass
    elif os.path.isfile(dirName):
        raise OSError("cannot create the direcory '%s'"
                      "because there exists already "
                      "a file with that name" % newdir)
    else:
        head, tail = os.path.split(dirName)
        if head and not os.path.isdir(head):
            ensureDir(head)
        if tail:
            os.mkdir(dirName)

def calcRelURL(toURL, fromURL):
    """
    Calculates a relative URL.
    """
    fromURL  = urlparse(fromURL)
    toURL    = urlparse(toURL)
    if not fromURL.scheme == toURL.scheme: return urlunparse(toURL)
    if not fromURL.netloc == toURL.netloc: return urlunparse(toURL)

    fromPath = fromURL.path.split("/")
    toPath   = toURL.path.split("/")
    for j in xrange(len(fromPath) - 1): fromPath[j] += u"/"
    for j in xrange(len(toPath)   - 1): toPath[j] += u"/"

    # abs path: ['/', 'dir1/', ..., 'dirN/', 'file']
    # rel path: ['dir1/', ..., 'dirN/', 'file']
    # path with no file: ['dir1/', ..., 'dirN/', '']

    # find common path (but do not count file name)
    i = 0
    while True:
        if i >= len(fromPath) - 1: break
        if i >= len(toPath) - 1: break
        if not fromPath[i] == toPath[i]: break
        i = i + 1

    # a/b/c/d.html  --> ../../../d.html
    # a/b//c/d.html --> ../../../d.html
    for j in xrange(len(fromPath) - 1):
        if len(fromPath[j]) > 1: fromPath[j] = u"../"
        else:                    fromPath[j] = u""

    fromPath = fromPath[i:-1]
    toPath = toPath[i:]
    relPath = u"".join(fromPath) + "".join(toPath)

    return urlunparse(("", "", relPath, "", "", toURL.fragment))

def walkNodes(rootNode, nodeType = None, nodeBarrier = None):
    if nodeBarrier and rootNode.isA(nodeBarrier):
        return
    for n in rootNode.getChildren():
        for m in walkNodes(n, nodeType, nodeBarrier):
            yield m
    if not nodeType or rootNode.isA(nodeType):
        yield rootNode

def walkAncestors(leafNode, nodeType = None):
    if not nodeType or leafNode.isA(nodeType):
        yield leafNode
    p = leafNode.getParent()
    if p:
        for m in walkAncestors(p, nodeType):
            yield m

# --------------------------------------------------------------------
class DocLocation:
# --------------------------------------------------------------------
    """
    A location consisting of a URL (file), a row number, and a column number.
    """
    def __init__(self, URL, row, column):
        self.URL = URL
        self.row = row
        self.column = column

    def __str__(self):
        return "%s:%d:%d" % (self.URL,
                             self.row,
                             self.column)

# --------------------------------------------------------------------
class DocError(BaseException):
# --------------------------------------------------------------------
    """
    An error consisting of a stack of locations and a message.
    """
    def __init__(self, message):
        BaseException.__init__(self,message)
        self.locations = []

    def __str__(self):
        str = ""
        if len(self.locations) > 0:
            for i in xrange(len(self.locations)-1,0,-1):
                str += "included from %s:\n" % self.locations[i]
            return str + "%s: error: %s" % (self.locations[0], BaseException.__str__(self))
        else:
            return self.message

    def appendLocation(self, location):
        self.locations.append(location)
        return self

# --------------------------------------------------------------------
class makeGuard(object):
# --------------------------------------------------------------------
    """
    Decorates the method of a DocNode object so that,
    on raising a DocError exception, the location of the node
    is appended to it.
    """

    def __init__(self, func):
        self.func = func

    def __call__(self, obj, *args, **keys):
        try:
            self.func(obj, *args, **keys)
        except DocError, e:
            if len(e.locations) == 0:
                e.appendLocation(obj.getLocation())
            raise e
        except:
            raise

    def __get__(self, obj, type=None):
        return types.MethodType(self, obj, type)

# --------------------------------------------------------------------
class DocBareNode:
# --------------------------------------------------------------------
    """
    A node of the document tree without parent, children, or any
    other attribute. It is used to implement common leaf nodes such
    as text chunks.
    """
    def __init__(self): pass

    def isA(self, classInfo):
        """
        Returns TRUE if the node is of class CLASSINFO.
        """
        return isinstance(self, classInfo)

    def getChildren(self):
        """
        Returs an empty list
        """
        return []

    def setParent(self, parent): pass
    def getPublishDirName(self): pass
    def getPublishFileName(self): pass
    def getPublishURL(self): pass
    def visit(self, generator): pass
    def publish(self, generator, pageNode = None): pass
    def publishIndex(self, gen, inPage, activePageNodes, full=False): return False

# --------------------------------------------------------------------
class DocNode(DocBareNode):
# --------------------------------------------------------------------
    """
    A node of the document with a parent, childern, attributes, and
    additional meta-information such as the location
    of the XML element that caused this node to be generated.
    """
    def __init__(self, attrs, URL, locator):
        self.parent = None
        self.children = []
        self.attrs = attrs
        self.sourceURL = None
        self.sourceRow = None
        self.sourceColumn = None
        if attrs.has_key('id'):
            self.id = attrs['id']
        else:
            self.id = getUniqueNodeID()
        self.sourceURL = URL
        if locator:
            self.sourceRow = locator.getLineNumber()
            self.sourceColumn = locator.getColumnNumber()
        nodeIndex[self.id] = self

    def __str__(self):
        return "%s:%s -> %s" % (self.getLocation(), self.getID(), self.getPublishURL())

    def dump(self):
        """
        Recusively dump the tree of nodes, for debugging purposes.
        """
        depth = self.getDepth()
        print " " * depth, self
        for x in self.children: x.dump()

    def getID(self):
        """
        Return the node ID.
        """
        return self.id

    def getParent(self):
        """
        Return the node parent.
        """
        return self.parent

    def getChildren(self):
        """
        Return the list of node children.
        """
        return self.children

    def getAttributes(self):
        """
        Return the dictionary of node attributes.
        """
        return self.attrs

    def getDepth(self):
        """
        Return the depth of the node in the tree.
        """
        if self.parent:
            return self.parent.getDepth() + 1
        else:
            return 0

    def setParent(self, parent):
        """
        Set the parent of the node.
        """
        self.parent = parent

    def adopt(self, orfan):
        """
        Adds ORFAN to the node children and make the node the parent
        of ORFAN. ORFAN can also be a sequence of orfans.
        """
        self.children.append(orfan)
        orfan.setParent(self)

    def findAncestors(self, nodeType = None):
        """
        Return the node ancestors of type NODETYPE. If NODETYPE is
        None, returns all ancestors.
        """
        if nodeType is None:
            nodeType = DocNode
        if self.parent:
            if self.parent.isA(nodeType):
                found = [self.parent]
            else:
                found = []
            found = found + self.parent.findAncestors(nodeType)
            return found
        return []

    def findChildren(self, nodeType = None):
        """
        Returns the node chldren of type NODTYPE. If NODETYPE is None,
        returns all children.
        """
        if nodeType is None:
            nodeType = DocNode
        return [x for x in self.children if x.isA(nodeType)]

    def getLocation(self):
        """
        Get the location (file, row number, and column number)
        where this node was instanitated
        """
        location = DocLocation(self.sourceURL,
                               self.sourceRow,
                               self.sourceColumn)
        if self.parent:
            parentLocation = self.parent.getLocation()
            if location.URL is None: location.URL = parentLocation.URL
            if location.row is None: location.URL = parentLocation.row
            if location.column is None: location.URL = parentLocation.column
        return location

    def getPublishDirName(self):
        """
        Returns the publish dir name of the parent.
        """
        if self.parent:
            return self.parent.getPublishDirName()
        return None

    def getPublishFileName(self):
        """
        Returns NONE.
        """
        return None

    def getPublishURL(self):
        """
        Returns NONE.
        """
        return None

    def visit(self, generator):
        """
        Recursively calls VISIT() on its children.
        """
        for c in self.getChildren():
            c.visit(generator)
        return None

    def publish(self, generator, pageNode = None):
        """
        Recursively calls PUBLISH() on its children.
        """
        for c in self.getChildren():
            c.publish(generator, pageNode)
        return None

    publish = makeGuard(publish)

    def publishIndex(self, gen, inPage, activePageNodes, full=False):
        """
        Recursively calls PUBLISHINDEX() on its children.
        """
        hasIndexedChildren = False
        for c in self.getChildren():
            hasIndexedChildren = c.publishIndex(gen, inPage, activePageNodes, full) \
                or hasIndexedChildren
        return hasIndexedChildren

    def publishTableOfContents(self, gen, pageNode):
        """
        Create a TOC corresponding to the H1, H2, ... tags in a DocPage."
        """
        gen.putString("<div class='toc'>\n")
        gen.putString("<h3>Table of Contents</h3>")
        previousLevel = 0
        for q in pageNode.getChildren():
            for x in walkNodes(q, DocHtmlElement, DocPage):
                if x.tag not in ['h1', 'h2', 'h3', 'h4', 'h5']: continue
                level = int(x.tag[1]) # e.g. h2 -> level = 2
                title = "".join([y.text for y in walkNodes(x, DocHtmlText)])
                while previousLevel < level:
                    gen.putString("<ul>")
                    previousLevel += 1
                while previousLevel > level:
                    gen.putString("</ul>\n")
                    previousLevel -= 1
                gen.putString('<li class="level%d">'
                             '<a href="#%s">%s</a>'
                             '</li>\n' % (level, x.id, title))
            while previousLevel > 0:
                gen.putString("</ul>\n")
                previousLevel -= 1
        gen.putString("</div><!-- Table of contents -->\n")

    def expandAttr(self, value, pageNode):
        """
        Expand an attribute by substituting any directive with its value.
        """
        xvalue = ""
        next = 0
        for m in re.finditer("%[-\w._#:]+;", value):
            if next < m.start():
                xvalue += value[next : m.start()]
            next = m.end()
            directive = value[m.start()+1 : m.end()-1]
            mo = re.match('pathto:(.*)', directive)
            if mo:
                toNodeID = mo.group(1)
                toNodeURL = None
                if nodeIndex.has_key(toNodeID):
                    toNodeURL = nodeIndex[toNodeID].getPublishURL()
                if toNodeURL is None:
                    print "%s: warning: could not cross-reference '%s'" % (self.getLocation(), toNodeID)
                    toNodeURL = toNodeID
                fromPageURL = pageNode.getPublishURL()
                xvalue += calcRelURL(toNodeURL, fromPageURL)
                continue
            mo = re.match('env:(.*)', directive)
            if mo:
                envName = mo.group(1)
                if envName in os.environ:
                    xvalue += os.environ[envName]
                else:
                    print "%s: warning: the environment variable '%s' not defined" % (self.getLocation(), envName)
                continue
            mo = re.match('dox:(.*)', directive)
            if mo:
                if doxygenIndex is None:
                    if opts.verb > 1:
						print "%s: warning: no Doxygen tag file loaded, skipping this directive." % self.getLocation()
                    continue
                if not mo.group(1) in doxygenIndex.index:
                    print "%s: warning: the ID %s was not found in the Doxygen tag file." % (self.getLocation(), mo.group(2))
                    continue
                toNodeURL = getDoxygenURL(mo.group(1))
                fromPageURL = pageNode.getPublishURL()
                xvalue += calcRelURL(toNodeURL, fromPageURL)
                continue
            raise DocError("unknown directive '%s' found while expanding an attribute" % directive)
        if next < len(value): xvalue += value[next:]
        #print "EXPAND: ", value, " -> ", xvalue
        return xvalue

# --------------------------------------------------------------------
class DocInclude(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)
        if not attrs.has_key("src"):
            raise DocError("include missing 'src' attribute")
        self.filePath = attrs["src"]

    def __str__(self):
        return DocNode.__str__(self) + ":<web:include src=%s>" \
            % xml.sax.saxutils.quoteattr(self.filePath)

# --------------------------------------------------------------------
class DocDir(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)
        if not attrs.has_key("name"):
            raise DocError("dir tag missing 'name' attribute")
        self.dirName = attrs["name"]

    def __str__(self):
        return DocNode.__str__(self) + ":<web:dir name=%s>" \
            % xml.sax.saxutils.quoteattr(self.dirName)

    def getPublishDirName(self):
        return self.parent.getPublishDirName() + self.dirName + os.sep

    def visit(self, generator):
        generator.changeDir(self.dirName)
        DocNode.visit(self, generator)
        generator.parentDir()

# --------------------------------------------------------------------
class DocGroup(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

    def __str__(self):
        return DocNode.__str__(self) + ":<web:group>"

# --------------------------------------------------------------------
class DocCDATAText(DocBareNode):
# --------------------------------------------------------------------
    def __init__(self, text):
        DocBareNode.__init__(self)
        self.text = text

    def __str__(self):
        return DocNode.__str__(self) + ":CDATA text:" + self.text

    def publish(self, gen, pageNode = None):
        gen.putString(self.text)

# --------------------------------------------------------------------
class DocCDATA(DocNode):
# --------------------------------------------------------------------
    def __init__(self):
        DocNode.__init__(self, {}, None, None)

    def __str__(self):
        return DocNode.__str__(self) + ":CDATA"

    def publish(self, gen, pageNode = None):
        gen.putString("<![CDATA[")
        DocNode.publish(self, gen, pageNode)
        gen.putString("]]>")

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocHtmlText(DocBareNode):
# --------------------------------------------------------------------
    def __init__(self, text):
        DocBareNode.__init__(self)
        self.text = text

    def __str__(self):
        return DocNode.__str__(self) + ":text:'" + \
            self.text.encode('utf-8').encode('string_escape') + "'"

    def publish(self, gen, pageNode = None):
        # find occurences of %directive; in the text node and do the
        # appropriate substitutions
        next = 0
        for m in re.finditer("%(\w+)(?::([-\w._#]+))?;", self.text):
            if next < m.start():
                gen.putXMLString(self.text[next : m.start()])
            next = m.end()
            directive = self.text[m.start()+1 : m.end()-1]
            directive = m.group(1)
            if m.group(2):
                options = [x.strip().lower() for x in m.group(2).split(',')]
            else:
                options = []

            if directive == "content":
                pageNode.publish(gen, pageNode)

            elif directive == "pagestyle":
                for q in pageNode.getChildren():
                    for s in walkNodes(q, DocPageStyle, DocPage):
                        s.expand(gen, pageNode)

            elif directive == "pagescript":
                for q in pageNode.getChildren():
                    for s in walkNodes(q, DocPageScript, DocPage):
                        s.expand(gen, pageNode)

            elif directive == "pagetitle":
                gen.putString(pageNode.title)

            elif directive == "path":
                ancPages = [x for x in walkAncestors(pageNode, DocPage)]
                plain=False
                for option in options:
                    if option=="plain":
                        plain=True
                    else:
                        print "warning: ignoring unknown option '%s' while expanding 'path'" % option
                if ancPages is not None:
                    for i,p in enumerate(reversed(ancPages)):
                        if plain:
                            if i > 0: gen.putString(" > ")
                            gen.putString(p.title)
                        else:
                            if i > 0: gen.putString("<span class='separator'>></span>")
                            gen.putString("<span class='page'><a href=")
                            gen.putXMLAttr(
                                pageNode.expandAttr("%%pathto:%s;" % p.getID(), pageNode))
                            gen.putString(">%s</a></span>" % p.title)

            elif directive == "navigation":
                gen.putString("<ul>\n")
                # get the branch of DocPage nodes from the site root to this page
                activePageNodes = [x for x in walkAncestors(pageNode, DocPage)]
                # find the root site node and publish the contents
                siteNode = walkAncestors(pageNode, DocSite).next()
                siteNode.publishIndex(gen, pageNode, activePageNodes, True)
                gen.putString("</ul>\n")

            elif directive == "tableofcontents":
                pageNode.publishTableOfContents(gen, pageNode)

            elif directive == "env":
                envName = m.group(2)
                if envName in os.environ:
                    gen.putString(os.environ[envName])
                else:
                    print "warning: environment variable '%s' not defined" % envName
            else:
                print "warning: ignoring unknown directive '%s'" % label
        if next < len(self.text):
            gen.putXMLString(self.text[next:])


# --------------------------------------------------------------------
class DocCodeText(DocBareNode):
# --------------------------------------------------------------------
    def __init__(self, text):
        DocBareNode.__init__(self)
        self.text = text

    def __str__(self):
        return DocNode.__str__(self) + ":text:'" + \
            self.text.encode('utf-8').encode('string_escape') + "'"

# --------------------------------------------------------------------
class DocCode(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL = None, locator = None):
        DocNode.__init__(self, attrs, URL, locator)
        self.type = "plain"
        if attrs.has_key("type"): self.type = attrs["type"]

    def __str__(self):
        str = "<web:precode"
        for k, v in self.attrs.items():
            str = str + " " + k + "='" + xml.sax.saxutils.escape(v) + "'"
            str = str + "> type = " + self.type
        return DocNode.__str__(self) + ":" + str

    def publish(self, gen, pageNode = None):
        code = ""
        for n in self.getChildren():
            if n.isA(DocCodeText):
                code = code + n.text
        if has_pygments and not self.type == "plain":
            try:
                lexer = pygments.lexers.get_lexer_by_name(self.type)
                gen.putString(pygments.highlight(code,
                                                 lexer,
                                                 pygments.formatters.HtmlFormatter()))
            except pygments.util.ClassNotFound:
                print "warning: could not find a syntax highlighter for '%s'" % self.type
                gen.putString("<pre>" + code + "</pre>")
        else:
            gen.putString("<pre>" + code + "</pre>")
        DocNode.publish(self, gen, pageNode)

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocHtmlElement(DocNode):
# --------------------------------------------------------------------
    def __init__(self, tag, attrs, URL = None, locator = None):
        DocNode.__init__(self, attrs, URL, locator)
        self.tag = tag

    def __str__(self):
        str = "<html:" + self.tag
        for k, v in self.attrs.items():
            str = str + " " + k + "='" + xml.sax.saxutils.escape(v) + "'"
        str = str + ">"
        return DocNode.__str__(self) + ":" + str

    def getPublishURL(self):
        anc = self.findAncestors(DocPage)
        if len(anc) == 0: return None
        return anc[0].getPublishURL() + "#" + self.id

    def publish(self, gen, pageNode = None):
        gen.putString("<")
        gen.putString(self.tag)
        # make sure headings have and id (for ToCs)
        if self.tag in ['h1', 'h2', 'h3', 'h4', 'h5'] and \
           not "id" in self.attrs:
            self.attrs["id"] = self.id ;
        for name, value in self.attrs.items():
            gen.putString(" ")
            gen.putString(name)
            gen.putString("=")
            gen.putXMLAttr(self.expandAttr(value, pageNode))
        if self.tag == 'br':
            # workaround for browser that do not like <br><br/>
            gen.putString("/>")
        elif self.tag == 'code':
            # expand tags such as <code>vl_function</code> as links
            gen.putString("/>")
            text = "".join([y.text for y in walkNodes(self, DocHtmlText)])
            ok = nodeIndex.has_key(text)
            if ok: gen.putString("<a href=" + self.expandAttr("%%pathto:%s;" % text, pageNode) + ">")
            DocNode.publish(self, gen, pageNode)
            if ok: gen.putString("</a>")
            gen.putString("</")
            gen.putString(self.tag)
            gen.putString(">")
        else:
            gen.putString(">")
            DocNode.publish(self, gen, pageNode)
            gen.putString("</")
            gen.putString(self.tag)
            gen.putString(">")

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocTemplate(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

# --------------------------------------------------------------------
class DocPageStyle(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

    def publish(self, gen, pageNode = None):
        return None

    def expand(self, gen, pageNode = None):
        sa = self.getAttributes()
        if sa.has_key("href"):
            gen.putString("<link rel=\"stylesheet\" type=")
            if sa.has_key("type"):
                gen.putXMLAttr(self.expandAttr(sa["type"], pageNode))
            else:
                gen.putString("\"text/css\" ")
            gen.putString("href=")
            gen.putXMLAttr(self.expandAttr(sa["href"], pageNode))
            gen.putString("></link>\n")
        else:
            gen.putString("<style rel=\"stylesheet\" type=")
            if sa.has_key("type"):
                gen.putXMLAttr(self.expandAttr(sa["type"], pageNode))
            else:
                gen.putString("\"text/css\" ")
	        gen.putString(">")
            DocNode.publish(self, gen, pageNode)
            gen.putString("</style>\n")

    expand = makeGuard(expand)

# --------------------------------------------------------------------
class DocPageScript(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

    def publish(self, gen, pageNode = None):
        return None

    def expand(self, gen, pageNode = None):
        sa = self.getAttributes()
        gen.putString("<script type=")
        if sa.has_key("type"):
            gen.putXMLAttr(self.expandAttr(sa["type"], pageNode))
            gen.putString(" ")
        else:
            gen.putString("\"text/javascript\" ")
        if sa.has_key("src"):
            gen.putString("src=")
            gen.putXMLAttr(self.expandAttr(sa["src"], pageNode))
        gen.putString(">")
        DocNode.publish(self, gen, pageNode)
        gen.putString("</script>\n")

    expand = makeGuard(expand)

# --------------------------------------------------------------------
class DocPage(DocNode):
# --------------------------------------------------------------------
    counter = 0

    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)
        DocPage.counter = 1 + DocPage.counter
        self.templateID = "template.default"
        self.name  = "page%d" % DocPage.counter
        self.title = "untitled"
        self.hide = False

        for k, v in self.attrs.items():
            if k == 'src':
                self.title = v
            elif k == 'name':
                self.name = v
            elif k == 'id':
                pass
            elif k == 'title':
                self.title = v
            elif k == 'hide':
                self.hide = (v.lower() == 'yes')
            else:
                raise DocError(
                    "web:page cannot have '%s' attribute" % k)

    def __str__(self):
        return DocNode.__str__(self) + ":<web:page name='%s' title='%s'>" \
            % (xml.sax.saxutils.escape(self.name),
               xml.sax.saxutils.escape(self.title))

    def getPublishFileName(self):
        return self.name + ".html"

    def getPublishURL(self):
        siteNode = self.findAncestors(DocSite)[0]
        return siteNode.getPublishURL() + \
            self.getPublishDirName() + \
            self.getPublishFileName()

    def visit(self, generator):
        generator.open(self.getPublishFileName())
        templateNode = nodeIndex[self.templateID]
        templateNode.publish(generator, self)
        generator.close()
        DocNode.visit(self, generator)

    def publish(self, generator, pageNode = None):
        if pageNode is self:
            # this is the page being published, so go on
            if opts.verb: print 'Publishing \'%s\''  % self.getPublishURL()
            DocNode.publish(self, generator, pageNode)
        # otherwise this page has been encountered recursively
        # during publishing
        return None

    def publishIndex(self, gen, inPage, activePageNodes, full=False):
        if self.hide: return False
        active = (self in activePageNodes)
        if active:
            activeLeaf = (activePageNodes.index(self) == 0)#len(activePageNodes)-1)
        else:
            activeLeaf = False
        gen.putString("<li")
        if active: gen.putString(" class='active'")
        if activeLeaf: gen.putString(" class='activeLeaf'")
        gen.putString("><a href=")
        gen.putXMLAttr(
            self.expandAttr("%%pathto:%s;" % self.getID(), inPage))
        gen.putString(">")
        gen.putXMLString(self.title)
        gen.putString("</a>\n")
        # Generate recursively the index of the children
        # This may or may not produce results; if not we need to backtrack,
        # so we save the position of the generator.
        pos = gen.tell()
        gen.putString("<ul>\n")
        if active or full:
            notEmpty = DocNode.publishIndex(self, gen, inPage, activePageNodes, full)
        else:
            notEmpty = False
        if notEmpty:
            gen.putString("</ul>")
        else:
            gen.seek(pos)
        gen.putString("</li>\n")
        return True

# --------------------------------------------------------------------
class DocSite(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)
        self.siteURL = "http://www.foo.org/"
        self.outDir = "html"

    def __str__(self):
        return DocNode.__str__(self) + ":<web:site>"

    def getPublishURL(self):
        return self.siteURL

    def setPublishURL(self, url):
        self.siteURL = url

    def getPublishDirName(self):
        return ""

    def getOutDir(self):
        return self.outDir

    def setOutDir(self, outDir):
        self.outDir = outDir

    def publish(self):
        generator = Generator(self.outDir)
        self.visit(generator)

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class Generator:
# --------------------------------------------------------------------
    def __init__(self, rootDir):
        ensureDir(rootDir)
        self.fileStack = []
        self.dirStack = [rootDir]
        ensureDir(rootDir)
        #print "CD ", rootDir

    def open(self, filePath):
        filePath = os.path.join(self.dirStack[-1], filePath)
        fid = open(filePath, "w")
        self.fileStack.append(fid)
        fid.write(DOCTYPE_XHTML_TRANSITIONAL)
        #print "OPEN ", filePath

    def putString(self, str):
        fid = self.fileStack[-1]
        try:
            encoded = str.encode('utf-8')
            fid.write(encoded)
        except (UnicodeEncodeError, IOError), e:
            print str
            raise DocError("writing text:"  + e.__str__())
        except:
            raise

    def putXMLString(self, str):
        fid = self.fileStack[-1]
        xstr = xml.sax.saxutils.escape(str, mapUnicodeToHtmlEntity)
        try:
            fid.write(xstr.encode('utf-8'))
        except (UnicodeEncodeError, IOError), e:
            raise DocError("writing XML-escaped string:"  + e.__str__())
        except:
            raise

    def putXMLAttr(self, str):
        fid = self.fileStack[-1]
        xstr = xml.sax.saxutils.quoteattr(str)
        try:
            fid.write(xstr.encode('utf-8'))
        except (UnicodeEncodeError, IOError), e:
            raise DocError("writing XML-escaped attribute:"  + e.__str__())
        except:
            raise

    def close(self):
        self.fileStack.pop().close()
        #print "CLOSE"

    def changeDir(self, dirName):
        currentDir = self.dirStack[-1]
        newDir = os.path.join(currentDir, dirName)
        ensureDir(newDir)
        self.dirStack.append(newDir)
        #print "CD ", newDir

    def parentDir(self):
        self.dirStack.pop()
        #print "CD .."

    def tell(self):
        fid = self.fileStack[-1]
        return fid.tell()

    def seek(self, pos):
        fid = self.fileStack[-1]
        fid.seek(pos)

# --------------------------------------------------------------------
class DocHandler(ContentHandler):
# --------------------------------------------------------------------

    def __init__(self):
        ContentHandler.__init__(self)
        self.rootNode = None
        self.stack = []
        self.locatorStack = []
        self.filePathStack = []
        self.inDTD = False

    def resolveEntity(self, publicid, systemid):
        """
        Resolve XML entities by mapping to a local copy of the (X)HTML
        DTDs.
        """
        return open(os.path.join(
                os.path.dirname(__file__),
                'dtd/xhtml1',
                systemid[systemid.rfind('/')+1:]), "rb")

    def lookupFile(self, filePath):
        if os.path.exists(filePath):
            return filePath
        if filePath[0] == '/':
            return None
        for path in self.filePathStack:
            dir = os.path.dirname(path)
            qualFilePath = os.path.join(dir, filePath)
            if os.path.exists(qualFilePath):
                return qualFilePath
        return None

    def makeError(self, message):
        e = DocError(message)
        for i in xrange(len(self.filePathStack)-1,-1,-1):
            URL = self.filePathStack[i]
            locator = self.locatorStack[i]
            e.appendLocation(DocLocation(URL,
                                         locator.getLineNumber(),
                                         locator.getColumnNumber()))
        return e

    def startElement(self, name, attrs):
        """
        SAX interface: starting of XML element.
        The function creates a new document node, i.e. a specialized
        class of DocNode for the type of XML element encountered. It then
        appends it as the head of the parsing stack for further processing."
        """
        # convert attrs to a dictionary (implicitly copies as required by the doc)
        attrs_ = {}
        for k, v in attrs.items():
            attrs_[k] = v
        attrs = attrs_

        URL = self.getCurrentFileName()
        locator = self.getCurrentLocator()

        # The <web:include> element is not parsed recusrively; instead
        # it simply switches to parsing the specified file.
        if name == "include":
            if not attrs.has_key("src"):
                raise self.makeError("<web:include> lacks the 'src' attribute")
            filePath = attrs["src"]
            qualFilePath = self.lookupFile(filePath)
            if qualFilePath is None:
                raise self.makeError("the file '%s' could not be found while expanding <web:include>" % filePath)
            if opts.verb: print "Parsing '%s'" % qualFilePath
            if attrs.has_key("type"):
                includeType = attrs["type"]
            else:
                includeType = "webdoc"
            if includeType == "webdoc":
                self.load(qualFilePath)
            elif includeType == "text":
                self.characters(open(qualFilePath, 'r').read())
            else:
                raise makeError("'%s' is not a valid <web:include> type" % includeType)
            return

        if len(self.stack) == 0:
            parent = None
        else:
            parent = self.stack[-1]
        node = None

        if name == "site":
            node = DocSite(attrs, URL, locator)
        elif name == "page":
            node = DocPage(attrs, URL, locator)
        elif name == "dir":
            node = DocDir(attrs, URL, locator)
        elif name == "template":
            node = DocTemplate(attrs, URL, locator)
        elif name == "pagestyle":
            node = DocPageStyle(attrs, URL, locator)
        elif name == "pagescript":
            node = DocPageScript(attrs, URL, locator)
        elif name == "group":
            node = DocGroup(attrs, URL, locator)
        elif name == "precode":
            node = DocCode(attrs, URL, locator)
        else:
            node = DocHtmlElement(name, attrs, URL, locator)

        if parent: parent.adopt(node)
        self.stack.append(node)

    def endElement(self, name):
        """
        SAX interface: closing of XML element.
        """
        if name == "include":
            return
        node = self.stack.pop()
        if len(self.stack) == 0:
            self.rootNode = node

    def load(self, qualFilePath):
        self.filePathStack.append(qualFilePath)
        parser = xml.sax.make_parser()
        parser.setContentHandler(self)
        parser.setEntityResolver(self)
        parser.setProperty(xml.sax.handler.property_lexical_handler, self)
        try:
            parser.parse(qualFilePath)
        except xml.sax.SAXParseException, e:
            raise self.makeError("XML parsing error: %s" % e.getMessage())

    def setDocumentLocator(self, locator):
        """SAX interface: This is called when a new file is parsed to set the locator object."""
        self.locatorStack.append(locator)

    def getCurrentLocator(self):
        if len(self.locatorStack) > 0:
            return self.locatorStack[-1]
        else:
            return None

    def characters(self, content):
        """
        SAX interface: characters.
        """
        parent = self.stack[-1]
        if parent.isA(DocCDATA):
            node = DocCDATAText(content)
        elif parent.isA(DocCode):
            node = DocCodeText(content)
        else:
            node = DocHtmlText(content)
        parent.adopt(node)

    def ignorableWhitespace(self, ws):
        self.characters(ws)

    def getCurrentFileName(self):
        return self.filePathStack[-1]

    def endDocument(self):
        self.locatorStack.pop()
        self.filePathStack.pop()

    def startCDATA(self):
        node = DocCDATA()
        self.stack[-1].adopt(node)
        self.stack.append(node)

    def endCDATA(self):
        node = self.stack.pop()
        if len(self.stack) == 0:
            self.rootNode = node

    def comment(self, body):
        if self.inDTD: return
        node = DocCDATAText("<!--" + body + "-->")
        self.stack[-1].adopt(node)

    def startEntity(self, name): pass
    def endEntity(self, name): pass

    def startDTD(self, name, public_id, system_id):
        self.inDTD = True

    def endDTD(self):
        self.inDTD = False

# --------------------------------------------------------------------
def start(filePath, opts):
# --------------------------------------------------------------------
    global doxygenIndex
    global doxygenDir

    if not has_pygments and opts.verb:
        print "Warning: Pygments module not found: syntax coloring disabled."

    handler = DocHandler()
    try:
        handler.load(filePath)
    except DocError, e:
        print e
        sys.exit(-1)

    # configure site
    handler.rootNode.setOutDir(opts.outdir)
    handler.rootNode.setPublishURL(opts.siteurl)

    # load doxygen tag file
    if opts.doxytag:
        if opts.verb: print "Loading the Doxygen tag file", opts.doxytag
        try:
            doxygenIndex = Doxytag(opts.doxytag)
            doxygenDir = opts.doxydir
        except Exception, e:
            print "Error parsing the Doxygen tag file", opts.doxytag
            print e
            sys.exit(-1)

    if opts.verb > 2:
        print "== All pages =="
        for x in walkNodes(handler.rootNode, DocPage):
            print x

    if opts.verb: print "Publishing website..."
    try:
        handler.rootNode.publish()
    except DocError, e:
        print e
        sys.exit(-1)

    if opts.indexfile:
        if opts.verb: print "Storing the website index to", opts.indexfile
        try:
            f = open(opts.indexfile, 'w+')
            siteurl = nodeIndex['root'].getPublishURL()
            for (id,x) in sorted(nodeIndex.items()):
                if (x.isA(DocHtmlElement) or x.isA(DocPage)) and x.attrs.has_key('id'):
                    url = x.getPublishURL()
                    if not url: continue
                    print >>f, '%s|%s' % (x.attrs['id'],
                                          calcRelURL(url,siteurl))
            if doxygenIndex:
                for tag in sorted(doxygenIndex.index):
                    url = getDoxygenURL(tag)
                    print >>f, '%s|%s' % (tag,
                                          calcRelURL(url,siteurl))
        except Exception, e:
            print "Error writing the website index file"
            print e
            sys.exit(-1)

    sys.exit(0)

# --------------------------------------------------------------------
if __name__ == '__main__':
# --------------------------------------------------------------------
    usage = """webdoc [OPTIONS...] <DOC.XML>

--outdir    Set output directory
--verbose   Be verbose
--doxytag   Doxygen tag file
--doxydir   Doxygen documentation location
--profile   Collect and print profiling information
"""
    parser = OptionParser(usage=usage)
    parser.add_option(
        "-v", "--verbose",
        dest    = "verb",
        default = 0,
        action  = "count",
        help    = "print more debuging information")
    parser.add_option(
        "-o", "--outdir",
        dest    = "outdir",
        default = "html",
        action  = "store",
        help    = "write output to this directory")
    parser.add_option(
        "", "--doxytag",
        dest    = "doxytag",
        default = None,
        action  = "store",
        help    = "use this doxygen tag file")
    parser.add_option(
        "", "--doxydir",
        dest    = "doxydir",
        default = ".",
        action  = "store",
        help    = "find doxygen documentation here")
    parser.add_option(
        "", "--profile",
        dest    = "profile",
        default = False,
        action  = "store_true",
        help    = "run the profiler")
    parser.add_option(
        "", "--siteurl",
        dest = "siteurl",
        default = "",
        action = "store",
        help = "set the base URL of the website")
    parser.add_option(
        "", "--indexfile",
        dest = "indexfile",
        default = None,
        action = "store",
        help = "store the website index here")

    (opts, args) = parser.parse_args()

    if opts.profile:
        cProfile.run('start(args[0], opts)')
    else:
        start(args[0], opts)
