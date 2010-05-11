#!/usr/bin/python
# file:        mdoc.py
# author:      Brian Fulkerson and Andrea Vedaldi
# description: MDoc main

# AUTORIGHTS
# Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
#
# This file is part of VLFeat, available in the terms of the GNU
# General Public License version 2.

import sys, os, re, shutil
import subprocess, signal

from wikidoc import wikidoc
from formatter import Formatter
from optparse import OptionParser


excludeRegexList = []
format           = 'html'
verb             = 0
sitexml          = ""

usage = """usage: %prog [options] <basedir> <docdir>

Takes all .m files in basedir and its subdirectories and converts
them to html documentation, placing the results in docdir."""

parser = OptionParser(usage=usage)

parser.add_option(
    "-f", "--format",
    dest    = "format",
    default = "html",
    action  = "store",
    help    = "specify the output format (html, wiki, web)",
    metavar = "STRING")

parser.add_option(
    "-x", "--exclude",
    dest    = "excludeList",
    action  = "append",
    type    = "string",
    help    = "exclude files matching the specified regexp")

parser.add_option(
    "-v", "--verbose",
    dest    = "verb",
    default = False,
    action  = "store_true",
    help    = "print debug information")


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
    MFile('sub/file.m') represents a MATLAB M-File.
    """
    def __init__(self, basedir, dirname, name):
        funcname = os.path.splitext(name)[0]

        self.funcname = funcname.upper()
        self.path     = os.path.join(basedir, dirname, name)
        self.mdocname = os.path.join(dirname, funcname).replace(os.path.sep, '_')
        self.webname  = os.path.join(dirname, funcname).replace(os.path.sep, '.')
        self.htmlname = self.mdocname + '.html'
        self.wikiname = 'MDoc_' + (os.path.join(dirname, funcname)
                                   .upper().replace(os.path.sep, '_'))

        self.prev = None
        self.next = None
        self.node = None

    def getId (self, format='html'):
        if format == 'html':
            return self.htmlname
        elif format == 'web':
            return self.webname
        elif format == 'wiki':
            return self.wikiname

    def getRef (self, format='html'):
        if format == 'html':
            return self.htmlname
        elif format == 'web':
            return '%pathto:' + self.webname + ';'
        elif format == 'wiki':
            return self.wikiname

    def __cmp__(self, other):
        return cmp(self.webname, other.webname)

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
    A Node N represents a node in the toolbox hierechy. A node is a
    directory in the toolbox hierarchy and contains both M-files and
    other sub-directories.
    """
    def __init__(self, dirname):
        self.dirname = dirname
        self.children = []
        self.mfiles   = []

    def addChildNode(self, node):
        "Add a child node (toolbox subdirectory) to this node"
        self.children.append(node)

    def addMFile(self, mfile):
        "Add a MATLAB M-File to this node"
        self.mfiles.append(mfile)
        mfile.node = self

    def toIndexPage(self, format='html', depth=1):
        "Converts the node hierarchy rooted here into an index."
        page = ""
        if format == 'html' or format == 'web':
            if len(self.mfiles) > 0:
                page += "<b>%s</b>" % (self.dirname.upper())
                page += "<ul>\n"
                for m in self.mfiles:
                    page += "<li>"
                    page += "<b><a href='%s'>%s</a></b>" % (m.getRef(format),
                                                            m.funcname)
                    page += " %s" % (m.brief)
                    page += "</li>"
                page += "</ul>\n"
        else:
            if len(self.mfiles) > 0:
                if depth > 1:
                    page += "=== %s ===\n" % (self.dirname.upper())
                for m in self.mfiles:
                    page += "* [[%s|%s]]" % (m.getRef(format), m.funcname)
                    page += " %s\n" % (m.brief)
        for n in self.children:
            page += n.toIndexPage(format, depth+1)
        return page

    def toIndexXML(self):
        xml = ""
        for m in self.mfiles:
            dirname = m.node.dirname.upper()
            if len(dirname) > 0:
                xml += \
                    "<page id='%s' name='%s' title='%s - %s' hide='yes'>" \
                    "<div class='mdoc'>" \
                    "<include src='%s'/></div></page>\n" % (m.getId('web'), m.funcname,
                                                            dirname,
                                                            m.funcname, m.htmlname)
            else:
                xml += \
                    "<page id='%s' name='%s' title='%s' hide='yes'>" \
                    "<div class='mdoc'>" \
                    "<include src='%s'/></div></page>\n" % (m.getId('web'), m.funcname,
                                                            m.funcname, m.htmlname)

        for n in self.children:
            xml += n.toIndexXML() ;
        return xml

    def __str__(self):
        s = "Node: %s\n" % self.dirname
        for m in self.mfiles:
            s += m.__str__()
        for n in self.children:
            s += n.__str__()
        return s

# --------------------------------------------------------------------
def depth_first(node):
# --------------------------------------------------------------------
    """
    depth_first(NODE) is a generator that implements a depth first
    visit of the node hierarchy rooted at NODE.
    """
    yield node
    for n in node.children:
        for m in depth_first(n):
            yield m
    return

# --------------------------------------------------------------------
def extract(path):
# --------------------------------------------------------------------
    """
    (BODY, FUNC, BRIEF) = extract(PATH) extracts the comment BODY, the
    function name FUNC and the brief description BRIEF from the MATLAB
    M-file located at PATH.
    """
    body         = []
    func         = ""
    brief        = ""
    seenfunction = False
    seenpercent  = False

    for l in open(path):

        # Remove whitespace and newline
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

        # remove leading `%' character
        line = line[1:] #
        body.append('%s\n' % line)

    # Extract header from body
    if len(body) > 0:
        head  = body[0]
        body  = body[1:]
        match = re.match(r"^\s*(\w+)\s*(\S.*)\n$", head)
        func  = match.group(1)
        brief = match.group(2)

    return (body, func, brief)


# --------------------------------------------------------------------
def xscan(baseDir, subDir=''):
# --------------------------------------------------------------------
    """
    NODE = xscan(BASEDIR) recusrively scans the directory BASEDIR and
    construct the toolbox hierarchy rooted at NODE.
    """

    node = Node(subDir)
    dir = os.listdir(os.path.join(baseDir, subDir))
    fileNames  = [f for f in dir if os.path.isfile(
                  os.path.join(baseDir, subDir, f))]
    subSubDirs = [s for s in dir if os.path.isdir (
                  os.path.join(baseDir, subDir, s))]
    fileNames.sort()

    # Scan M-FileNames
    for fileName in fileNames:
        # only m-files
        if not os.path.splitext(fileName)[1] == '.m':
            continue

        # skip if in the exclude list
        exclude = False
        for rx in excludeRegexList:
            fileRelPath = os.path.join(subDir, fileName)
            mo = rx.match(fileRelPath)
            if mo and (mo.end() - mo.start() == len(fileRelPath)):
                if verb:
                    print "mdoc: excluding ''%s''." % fileRelPath
                    exclude = True
        if exclude: continue

        node.addMFile(MFile(baseDir, subDir, fileName))

    # Scan sub-directories
    for s in subSubDirs:
        node.addChildNode(xscan(basedir, os.path.join(subDir, s)))

    return node

# --------------------------------------------------------------------
def breadCrumb(m):
# --------------------------------------------------------------------
    breadcrumb = "<ul class='breadcrumb'>"
    if format == 'web':
        breadcrumb += "<li><a href='%pathto:mdoc;'>Index</a></li>"
    else:
        breadcrumb += "<li><a href='index.html'>Index</a></li>"
    if m.prev: breadcrumb += "<li><a href='%s'>Prev</a></li>" % m.prev.getRef(format)
    if m.next: breadcrumb += "<li><a href='%s'>Next</a></li>" % m.next.getRef(format)
    breadcrumb += "</ul>"
    #breadcrumb += "<span class='path'>%s</span>" % m.node.dirname.upper()

    return breadcrumb

# --------------------------------------------------------------------
if __name__ == '__main__':
# --------------------------------------------------------------------

    #
    # Parse comand line options
    #

    (options, args) = parser.parse_args()

    if options.verb: verb = 1
    format = options.format

    print options.excludeList
    for ex in options.excludeList:
        rx = re.compile(ex)
        excludeRegexList.append(rx)

    if len(args) != 2:
        parser.print_help()
        sys.exit(2)

    basedir = args[0]
    docdir  = args[1]

    if not basedir.endswith('/'): basedir = basedir + "/"
    if not basedir.endswith('/'): docdir  = docdir + "/"

    if verb:
        print "mdoc: search path: %s" % basedir
        print "mdoc: output path: %s" % docdir
        print "mdoc: output format: %s" % format

    #
    # Search for mfiles
    #

    toolbox = xscan(basedir)

    #
    # Extract dictionaries of links and M-Files
    #

    linkdict = {}
    mfiles   = {}
    prev     = None
    next     = None
    for n in depth_first(toolbox):
        for m in n.mfiles:
            if prev:
                prev.next = m
                m.prev = prev
            prev = m
            func = m.funcname
            mfiles[func] = m
            linkdict[func] = m.getRef(format)
    if verb:
        print "mdoc: num mfiles: %d" % (len(mfiles))

    # Create output directory
    if not os.access(docdir, os.F_OK):
        os.makedirs(docdir)

    # ----------------------------------------------------------------
    #                          Extract comment block and run formatter
    # ----------------------------------------------------------------
    for (func, m) in mfiles.items():

        if format == 'wiki':
            outname = m.wikiname
        elif format == 'html':
            outname = m.htmlname
        elif format == 'web':
            outname = m.htmlname

        if verb:
            print "mdoc: generating %s from %s" % (outname, m.path)

        # extract comment block from file
        (lines, func, brief) = extract(m.path)

        m.brief = brief

        # Run formatter
        content = ""
        if len(lines) > 0:
            if format == 'wiki' :
                formatter = Formatter(lines, linkdict, 'wiki')
            else:
                formatter = Formatter(lines, linkdict, 'a')

            content = formatter.toDOM().toxml("UTF-8")
            content = content[content.find('?>')+2:]

        # add decorations
        if not format == 'wiki':
            content = breadCrumb(m) + content

        if format == 'web':
            content = "<group>\n" + content + "</group>\n"

        # save the result to an html file
        if format == 'wiki':
            f = open(os.path.join(docdir, m.wikiname), 'w')
        else:
            f = open(os.path.join(docdir, m.htmlname), 'w')
        f.write(content)
        f.close()

    # ----------------------------------------------------------------
    #                                                  Make index page
    # ----------------------------------------------------------------

    page = ""
    if format == 'html':
        pagename = 'index.html'
        page += toolbox.toIndexPage('html')
    elif format == 'web':
        pagename = 'mdoc.html'
        page += '<group>\n' + toolbox.toIndexPage('web') + '</group>\n'
    elif format =='wiki' :
        pagename = 'MDoc'
        page = "== Documentation ==\n"
        page += toolbox.toIndexPage('wiki')


    f = open(os.path.join(docdir, pagename), 'w')
    f.write(page)
    f.close()

    if format == 'web':
        f = open(os.path.join(docdir, "mdoc.xml"), 'w')
        f.write("<group>"+toolbox.toIndexXML()+"</group>\n")
        f.close()

    # ----------------------------------------------------------------
    #                                            Checkin files to wiki
    # ----------------------------------------------------------------
    def towiki(docdir, pagename):
        pagenamewiki = pagename + '.wiki'
        runcmd("cd %s ; mvs update %s" % (docdir, pagenamewiki))
        if verb:
            print "mdoc: converting", pagename, "to", pagenamewiki
        wikidoc(os.path.join(docdir, pagenamewiki),
                os.path.join(docdir, pagename))
        runcmd("cd %s ; mvs commit -M -m 'Documentation update' %s" % (docdir, pagenamewiki))

    if format == 'wiki' :
        try:
            towiki(docdir, pagename)
        except (KeyboardInterrupt, SystemExit):
            sys.exit(1)

        for (func, m) in mfiles.items():
            try:
                towiki(docdir, m.wikiname)
            except (KeyboardInterrupt, SystemExit):
                sys.exit(1)
