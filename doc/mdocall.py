#!/usr/bin/python
# file:        mdocall.py
# author:      Brian Fulkerson and Andea Vedaldi
# description: MDoc main

import sys, os, re
import subprocess, signal

from wikidoc import wikidoc
from m2ascii import convertfile
from formatter import Formatter
from optparse import OptionParser


parser = OptionParser()

parser.add_option(
    "-k", "--wiki", 
    dest    = "wikiformat",
    default = False,
    action  = "store_true",
    help    = "format file for wiki",
    metavar = "BOOL")

parser.add_option(
    "-v", "--verbose", 
    dest    = "verb",
    default = False,
    action  = "store_true",
    help    = "print debug informations")

wikiformat = False
verb = 0

htmlHead = """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type"
content="text/html; charset=iso-8859-1" />
<meta name="robots"
      content="all" />
<meta name="generator"
      content="mdoc" />
<meta name="generatorversion"
      content="alpha" />

<!-- Include VLWeb CSS stylesheet and JavaScript functions -->
<link rel="stylesheet"
      type="text/css"
      href="mdoc.css"/>
</head>
<body>
""" 

# --------------------------------------------------------------------
def usage():
# --------------------------------------------------------------------
    """
    usage() prints usage informations.
    """
    print "Usage:", sys.argv[0], "<basedir> <docdir>"
    print ""
    print "Takes all .m files in basedir and its subdirectories and converts"
    print "them to html documentation, placing the results in docdir."

# --------------------------------------------------------------------
def runcmd(cmd):
# --------------------------------------------------------------------
    """
    runcmd(CMD) runs the command CMD. The function waits for the
    command to complete and correctly react to Ctrl-C by stopping the
    process and raising an exception.
    """
    try:
        p = subprocess.Popen(cmd, shell=True)
        sts = os.waitpid(p.pid, 0)
    except (KeyboardInterrupt, SystemExit):
        os.kill(p.pid, signal.SIGKILL)
        raise

# --------------------------------------------------------------------
class MFile:
# --------------------------------------------------------------------    
    """
    MFile('sub/file.m')
    """
    def __init__(self, basedir, dirname, name):
        funcname = os.path.splitext(name)[0]
        
        self.funcname = funcname.upper()
        self.path     = os.path.join(basedir, dirname, name)
        self.mdocname = os.path.join(dirname, funcname).replace(os.path.sep, '_')
        self.htmlname = self.mdocname + '.html'
        self.wikiname = 'MDoc_' + os.path.join(dirname, funcname).upper().replace(os.path.sep, '_')

    def __str__(self):
        str  = "MFile: %s\n" % (self.funcname)
        str += "  path    : %s\n" % (self.path)
        str += "  mdocname: %s\n" % (self.mdocname)
        str += "  htmlname: %s\n" % (self.htmlname)
        str += "  wikiname: %s\n" % (self.wikiname)
        return str

# --------------------------------------------------------------------
class Node:
# --------------------------------------------------------------------    
    """
    """
    def __init__(self, dirname):
        self.dirname = dirname
        self.children = []
        self.mfiles   = []

    def addChildNode(self, node):
        self.children.append(node)

    def addMFile(self, mfile):
        self.mfiles.append(mfile)

    def toIndexPage(self,format,depth=1):
        page = ""
        if format == 'html':
            if len(self.mfiles) > 0:
                page += "<b>%s</b>" % (self.dirname.upper())
                page += "<ul>\n"
                for m in self.mfiles:
                    page += "<li>"
                    page += "<b><a href='%s'>%s</a></b>" % (m.htmlname, m.funcname)
                    page += " %s" % (m.brief)
                    page += "</li>"
                page += "</ul>\n"
        else:
            if len(self.mfiles) > 0:
                if depth > 1:
                    page += "=== %s ===\n" % (self.dirname.upper())
                for m in self.mfiles:
                    page += "* [[%s|%s]]" % (m.wikiname, m.funcname)
                    page += " %s\n" % (m.brief)
        for n in self.children:
            page += n.toIndexPage(format, depth+1)
        return page
                
    def __str__(self):
        s = "Node: %s\n" % self.dirname
        for m in self.mfiles:
            s += m.__str__()
        for n in self.children:
            s += n.__str__()
        return s

def depth_first(node):
    yield node
    for n in node.children:
        for m in depth_first(n):
            yield m
    return


# --------------------------------------------------------------------
def extract(path):
# --------------------------------------------------------------------
    body         = []
    func         = ""
    brief        = ""
    seenfunction = False
    seenpercent  = False

    for l in open(path):
        
        # remove whitespace and newline
        line = l.strip().lstrip()

        if line.startswith('%'): seenpercent = True
        if line.startswith('function'): 
            seenfunction = True
            continue
        if not line.startswith('%'):
            if (seenfunction and seenpercent) or not seenfunction:
                break
            else:
                continue

        # remove leading %
        line = line[1:] #
        body.append('%s\n' % line)

    #
    # Extract header from body
    #
    if len(body) > 0:
        head  = body[0]
        body  = body[1:]
        match = re.match(r"^\s*(\w+)\s*(\S.*)\n$", head)
        func  = match.group(1)
        brief = match.group(2)

    return (body, func, brief)


# --------------------------------------------------------------------
def xscan(basedir, dirname=''):
# --------------------------------------------------------------------
    """
    node = xscan(basedir) extracts the toolbox structure from basedir.
    """
    
    node = Node(dirname)

    dir   = os.listdir(os.path.join(basedir, dirname))
    files = [f for f in dir if os.path.isfile(os.path.join(basedir, dirname, f))]
    sdirs = [s for s in dir if os.path.isdir (os.path.join(basedir, dirname, s))]

    # M-Files
    for f in files:
        # only m-files
        if not os.path.splitext(f)[1] == '.m':
            continue

        mfile = MFile(basedir, dirname, f)    

        # nod demo/test
        func = mfile.funcname
        if func.find('DEMO') != -1 : continue
        if func.find('TEST') != -1 : continue
        
        node.addMFile(mfile)

    # Sub-directories
    for s in sdirs:
        node.addChildNode(xscan(basedir, os.path.join(dirname, s)))

    return node

# --------------------------------------------------------------------
def decorateHTML(content, funcname, brief):
# --------------------------------------------------------------------
    """
    decorateHTML(CONTENT, FUNCNAME, BRIEF)
    """
    breadcrumb =  "<ul id='breadcrumb'><li>"
    breadcrumb += "<a href='index.html'>Index</a>"
    breadcrumb += "</li></ul>"
    heading = "<h1>" + funcname + " <span>" + brief + "</span></h1>"
    
    x =  htmlHead + breadcrumb + heading + content
    x += "</body></html>"
    return x

# --------------------------------------------------------------------
def putCSS(docdir):
# --------------------------------------------------------------------
    content = """
body, html, div, dl, dt, dd, p
{
  margin: 0 ;
  padding: 0 ;
}

html { 
 background-color: black ;
 margin: 0 ;
 padding: 0 ;
}

body {
 max-width: 50em ;
 line-height: 1.4em ;
 margin: 0 auto ;
 background-color: white ;
 font-size: 10pt ;
 font-family: "Lucida Grande", Helvetica ;
}

p {
margin: 0.8em 0;
}

dt {
color: DarkOrange  ;
margin: 0 ;
}

dd {
margin: 0em 0em 0em 2em ;
}

dd p {
  margin-top: 0 ;
}

ul {
padding:0 ;
}

li {
padding: 0 ;
margin: 0 ;
margin-left: 1.5em ;
}

ul ul  {
margin-left:1em ;
margin-bottom: 1em ;
}

pre {
    font-size: 1em ;
    color: Teal ;
}

h1 {
  color: DarkBlue ;
  font-size: 1.2em ;
  border-bottom: 2px solid DarkBlue ;
  padding-left: 0.5em ;
  padding-bottom: 0.2em ;
}

h1 span {
  color: black ;
  font-size: 1em ;
  font-weight: normal ;
  margin-left: 0.5em ;
}

div.documentation {
  padding: 0em 2em 2em 2em ;
  max-width: 40em ;
}

#breadcrumb {
  display: block ;
  border-bottom: 1px solid #eee ;
  margin: 0 ;
  padding: 2px 0.5em ;
  font-size: 0.8em ;
}

#breadcrumb li {
  display: inline ;
  margin: 0 ;
  padding: 0 ;
  margin-right: 2em ;
}

"""
    f = open(os.path.join(docdir, "mdoc.css"), 'w')
    f.write(content)
    f.close()
    
# --------------------------------------------------------------------
if __name__ == '__main__':
# --------------------------------------------------------------------
    
    #
    # Parse comand line options
    #

    (options, args) = parser.parse_args()

    if options.verb: verb = 1
    wikiformat = options.wikiformat

    if len(args) != 2:
        usage()
        sys.exit(2)
    
    basedir = args[0]
    docdir  = args[1]
    if not basedir.endswith('/'): basedir = basedir + "/"
    if not basedir.endswith('/'): docdir  = docdir + "/"

    if verb:
        print "mdoc: search path: %s" % basedir
        print "mdoc: output path: %s" % docdir
        print "mdoc: wiki foramt: %d" % wikiformat 

    #
    # Search for mfiles
    #

    toolbox = xscan(basedir)

    #
    # Extract dictionaries of links and M-Files
    #

    linkdict = {}
    mfiles   = {}
    for n in depth_first(toolbox):
        for m in n.mfiles:
            func = m.funcname
            mfiles[func] = m
            if wikiformat:
                linkdict[func] = m.wikiname
            else:
                linkdict[func] = m.htmlname

    if verb:
        print "mdoc: num mfiles: %d" % (len(mfiles))

    #
    # Create output directory
    #

    if not os.access(docdir, os.F_OK):
        os.makedirs(docdir)

    if not wikiformat:
        putCSS(docdir)
                
    # ----------------------------------------------------------------
    #                          Extract comment block and run formatter
    # ----------------------------------------------------------------

    for (func, m) in mfiles.items():
        
        if wikiformat:
            outname = m.wikiname
        else:
            outname = m.htmlname

        if verb:
            print "mdoc: generating %s from %s" % (outname, m.path)

        # extract comment block from file
        (lines, func, brief) = extract(m.path)

        m.brief = brief ;

        # run formatter
        content = ""
        if len(lines) > 0:
            if wikiformat:
                formatter = Formatter(lines, linkdict, 'wiki')
            else:
                formatter = Formatter(lines, linkdict, 'a')

            content = formatter.toDOM().toxml("UTF-8")
            content = content[content.find('?>')+2:]

        # add decorations
        if not wikiformat:
            content = decorateHTML(content, m.funcname, brief)

        # save the result to an html file
        if wikiformat:
            f = open(os.path.join(docdir, m.wikiname), 'w')
        else:
            f = open(os.path.join(docdir, m.htmlname), 'w')
        f.write(content)
        f.close()
        
    # ----------------------------------------------------------------
    #                                                  Make index page
    # ----------------------------------------------------------------
    
    if not wikiformat:
        pagename = os.path.join(docdir, 'index.html')
        page = htmlHead 
        page += "<ul id='breadcrumb'></ul>"
        page += "<h1>Index of functions</h1>\n"
        page += "<div class='documentation'>"
        page += toolbox.toIndexPage('html')
        page += "</div></body></html>"
    else:
        pagename = os.path.join(docdir, 'index')
        page = "== Documentation ==\n"
        page += toolbox.toIndexPage('wiki')

    f = open(pagename, 'w')
    f.write(page)
    f.close()

    sys.exit(0)
    try:
        runcmd("mvs update %s" % wikiname)
        print "Converting", overviewname, "to", wikiname
        wikidoc(wikiname, overviewname)
        runcmd("mvs commit -M -m 'Documentation update' %s" % wikiname)
    except (KeyboardInterrupt, SystemExit):
        sys.exit(1)

    # checkin files to wiki
    for funcname in mfiles:
        (mname, htmlname, linkname, brief) = mfiles[funcname]
        wikiname = linkname + ".wiki"
        try:
            runcmd("mvs update %s" % wikiname)
            print "Converting", htmlname, "to", wikiname
            wikidoc(wikiname, htmlname)
            runcmd("mvs commit -M -m 'Documentation update' %s" % wikiname)
            # possibly add -M for minor revision
        except (KeyboardInterrupt, SystemExit):
            sys.exit(1)
