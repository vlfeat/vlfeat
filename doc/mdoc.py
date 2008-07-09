#!/usr/bin/python
# file:        mdoc.py
# author:      Brian Fulkerson and Andrea Vedaldi
# description: MDoc main

import sys, os, re, shutil
import subprocess, signal

from wikidoc import wikidoc
from formatter import Formatter
from optparse import OptionParser
from webdoc import WebSite, readText, PageGenerator

usage = """usage: %prog [options] <basedir> <docdir>

Takes all .m files in basedir and its subdirectories and converts
them to html documentation, placing the results in docdir."""

parser = OptionParser(usage=usage)

parser.add_option(
    "-w", "--wiki", 
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
    help    = "print debug information")

parser.add_option(
    "-s", "--site", 
    dest    = "sitexml",
    default = "",
    action  = "store",
    help    = "Site xml file")

wikiformat = False
verb = 0
sitexml = ""

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
        self.wikiname = 'MDoc_' + (os.path.join(dirname, funcname)
                                   .upper().replace(os.path.sep, '_'))

        self.prev = None
        self.next = None
        self.node = None

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
        mfile.node = self

    def toIndexPage(self,format,depth=1):
        page = ""
        if format == 'html':
            if len(self.mfiles) > 0:
                page += "<b>%s</b>" % (self.dirname.upper())
                page += "<ul>\n"
                for m in self.mfiles:
                    page += "<li>"
                    page += "<b><a href='%s'>%s</a></b>" % (m.htmlname, 
                                                            m.funcname)
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

# --------------------------------------------------------------------
def depth_first(node):
# --------------------------------------------------------------------
    """
    depth_first(node) is a depth first generator over the descendent
    of the specified node.
    """
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
    files = [f for f in dir if os.path.isfile(
            os.path.join(basedir, dirname, f))]
    sdirs = [s for s in dir if os.path.isdir (
            os.path.join(basedir, dirname, s))]

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

def breadCrumb(m):
    breadcrumb = "<div id='breadcrumb'>"
    breadcrumb += "<ul>"
    breadcrumb += "<li><a href='index.html'>Index</a></li>"
    if m.prev: breadcrumb += "<li><a href='%s'>Prev</a></li>" % m.prev.htmlname
    if m.next: breadcrumb += "<li><a href='%s'>Next</a></li>" % m.next.htmlname
    breadcrumb += "</ul>"
    breadcrumb += "<span class='path'>%s</span>" % m.node.dirname.upper()
    breadcrumb += "</div>"
    
    return breadcrumb

# --------------------------------------------------------------------
if __name__ == '__main__':
# --------------------------------------------------------------------
    
    #
    # Parse comand line options
    #

    (options, args) = parser.parse_args()

    if options.verb: verb = 1
    wikiformat = options.wikiformat
    sitexml = options.sitexml

    if len(args) != 2:
        parser.print_help()
        sys.exit(2)
    
    basedir = args[0]
    docdir  = args[1]
    sitedir = basedir
    if sitexml != "":
      if verb: print "loading site", sitexml
      site = WebSite()
      site.load(sitexml, "../")
      sitedir = os.path.dirname(sitexml)

    if not basedir.endswith('/'): basedir = basedir + "/"
    if not basedir.endswith('/'): docdir  = docdir + "/"

    if verb:
        print "mdoc: search path: %s" % basedir
        print "mdoc: output path: %s" % docdir
        print "mdoc: wiki format: %d" % wikiformat 
        print "mdoc: site xml path: %s" % sitexml

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

    # Copy stylesheet
    if not wikiformat and not sitexml == "":
        shutil.copy(
          os.path.join(sitedir, site.stylesheet), 
          os.path.join(docdir, site.stylesheet))
                
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
            content = breadCrumb(m) + content
            page = readText(os.path.join(sitedir, site.template))
            page = re.sub("%stylesheet;", site.stylesheet, page)
            page = re.sub("%pagetitle;", "VLFeat - Toolbox - %s" % m.funcname, page)
            page = re.sub("%title;", "<h1>VLFeat</h1>", page)
            page = re.sub("%subtitle;", "<h2>Toolbox - %s - %s</h2>" %
                (m.funcname, m.brief), page)
            page = re.sub("%index;", site.genHtmlIndex(site.root), page)
            page = re.sub("%content;", content, page);
            generator = PageGenerator(site)
            generator.feed(page)
            content = generator.output()

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
        pagename = 'index.html'
        page = readText(os.path.join(sitedir, site.template))
        content  = "<ul id='breadcrumb'></ul>"
        content += "<h1>Index of functions</h1>\n"
        content += "<div class='documentation'>"
        content += toolbox.toIndexPage('html')
        content += "</div>"
        page = re.sub("%stylesheet;", site.stylesheet, page)
        page = re.sub("%pagetitle;", "VLFeat - Toolbox", page)
        page = re.sub("%title;", "<h1>VLFeat</h1>", page)
        page = re.sub("%subtitle;", "<h2>Toolbox</h2>", page)
        page = re.sub("%index;", site.genHtmlIndex(site.root), page)
        page = re.sub("%content;", content, page);
        generator = PageGenerator(site)
        generator.feed(page)
        page = generator.output()
    else:
        pagename = 'MDoc'
        page = "== Documentation ==\n"
        page += toolbox.toIndexPage('wiki')

    f = open(os.path.join(docdir, pagename), 'w')
    f.write(page)
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
 
    if wikiformat:
        try:
            towiki(docdir, pagename)
        except (KeyboardInterrupt, SystemExit):
            sys.exit(1)
            
        for (func, m) in mfiles.items():
            try:
                towiki(docdir, m.wikiname)
            except (KeyboardInterrupt, SystemExit):
                sys.exit(1)
