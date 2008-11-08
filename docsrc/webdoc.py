#!/usr/bin/python
# file:        webdoc.py
# authors:     Andrea Vedaldi and Brian Fulkerson
# description: Utility to generate a website

# AUTORIGHTS

import sys, os, re, subprocess, signal, htmlentitydefs, shutil
from wikidoc import wikidoc
from optparse import OptionParser
from HTMLParser import HTMLParser
from xml.parsers.expat import ExpatError
from xml.dom import minidom 
import random

usage = """usage: %prog [options] DOCUMENT

Webdoc parses a Webdoc document DOCUMENT and produces a website, ready
for publication.

DOCUMENT is an XML file describing the website structure. DOCUMENT may
look something like this:

<site>
  <stylesheet src="web.css">
  <page id="google" href="http://www.google.com" title="Google">
  <page id="index" title="Home page" src="index.html">
    <page id="index.greetings" title="Greetings" src="greetings.html"/>
  <page>
</site>

The site is a collection of files with a hierarchical structure. Files
are things such as HTML pages, stylesheets and so on.

"""

# default page template
template = """
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  %stylesheet;
</head>
<body>
&title;
&subtitle;
&index;
&content;
</body>
"""

verb = 0
srcdir = ""
outdir = ""

parser = OptionParser(usage=usage)

parser.add_option(
    "-v", "--verbose", 
    dest    = "verb",
    default = False,
    action  = "store_true",
    help    = "print debug informations")

parser.add_option(
    "-o", "--outdir", 
    dest    = "outdir",
    default = "",
    action  = "store",
    help    = "write output to this directory")

# --------------------------------------------------------------------
def xmkdir(newdir):
# --------------------------------------------------------------------
    """
    xmkdir(NEWDIR) makes the directory NEWDIR if need, along with any
    intermediate subpath. Overwriting existing files raises 
    an exception.
    """
    if os.path.isdir(newdir):
        pass
    elif os.path.isfile(newdir):
        raise OSError("a file with the same name as the desired " \
                      "dir, '%s', already exists." % newdir)
    else:
        head, tail = os.path.split(newdir)
        if head and not os.path.isdir(head):
            xmkdir(head)
        if tail:
            os.mkdir(newdir)

# --------------------------------------------------------------------
def readText(fileName):
# --------------------------------------------------------------------
  """
  TEXT = readText(NAME) returns the content of file NAME.
  """
  text = ""
  try:
    file = open(fileName, 'r')
    text = file.read()
    file.close()
  except IOError:
    print "Warning! Could not open or read file '%s'" % fileName
  return text
  
# --------------------------------------------------------------------
def writeText(fileName, text):
# --------------------------------------------------------------------
  """
  writeText(NAME, TEXT) writes TEXT to the file NAME.
  """
  try:
    xmkdir(os.path.dirname(fileName))
    file = open(fileName, 'w')
    file.write(text)
    file.close()
  except IOError:
    print "Warning! Could not write text file '%s'" % fileName

# --------------------------------------------------------------------
def iterateChildNodesByTag(node, tag):
# --------------------------------------------------------------------
  """
  This generator iterates the childern of NODE that are XML elements
  matching TAG.
  """
  n = node.firstChild
  while n:
    if n.nodeType == n.ELEMENT_NODE and n.nodeName == tag:
      yield n
    n = n.nextSibling

# --------------------------------------------------------------------
def iterateChildNodes(node):
# --------------------------------------------------------------------
  """
  This generator iterates all the childern of NODE.
  """
  n = node.firstChild
  while n:
    if n.nodeType == n.ELEMENT_NODE:
      yield n
    n = n.nextSibling
    
# --------------------------------------------------------------------
def getAttribute(element, attr, default=None):
# --------------------------------------------------------------------
  """
  V=getAttribute(ELEMENT, ATTR) returns the value of the attribute
  ATTR of the XML element ELEMENT. If ELEMENT has no such attribute,
  the function returns None.
  
  V=getAttribute(ELEMENT, ATTR, DEFAULT) returns DEFAULT instead of
  None if the attribute is not found.
  """
  if element.hasAttribute(attr):
    return element.getAttribute(attr)
  else:
    return default

# --------------------------------------------------------------------
class MakeStatic:
# --------------------------------------------------------------------
  """
  The instance MakeStatitc(CALLABLE) is a wrapper that calls
  CALLABLE. It is used to define static methods in Python classes, such
  as in:

    class TheClass:
      def theStaticMethod():
        pass
      theStaticMethod = MakeStatic(theStaticMethod)
      
  Now one can use the syntams TheClass.theStatitcMethod().
  """
  
  def __init__(self, method):
    self.__call__ = method
  
# --------------------------------------------------------------------
class Thing:
# --------------------------------------------------------------------
  """
  A THING is an object which: (i) has a unique identifier code and
  (ii) belongs to a global namespace. THINGs hold global references to
  files and other objects. Example of THINGs include HTML elements and
  files.
  """
  directory = {}

  def genUniqueID():
    "Generate an unique ID for a new THING"
    while 1:
      id = "%010d" % int(random.random() * 1e10)
      if id not in Thing.directory: break
    return id

  def dumpDirectory():
    "Dump all THINGs to stardard output"
    for (id, file) in Thing.directory.iteritems():
      print file

  genUniqueID = MakeStatic(genUniqueID)
  dumpDirectory = MakeStatic(dumpDirectory)

  def __init__(self, id=None):
    if id == None:
      id = Thing.genUniqueID()
    self.id = id
    Thing.directory [self.id] = self

  def __str__(self):
    return "id:%s" % self.id

# --------------------------------------------------------------------
class HtmlElement(Thing):
# --------------------------------------------------------------------
  """
  HTMLELEMENT represents an HTML element in a page of the
  website. This is used only to create cross references and holds no
  data.
  """

  def __init__(self, page, htmlId, id=None):
    Thing.__init__(self, id)
    self.htmlId = htmlId
    self.page   = page
    
  def __str__(self):
    return "%s htmlId:%s" % (Thing.__str__(self), self.htmlId)

  def getPathFromRoot(self):
    return self.page.getPathFromRoot() + "#" + self.htmlId

  def getPathFrom(self, basedir):
    return self.page.getPathFrom(basedir) + "#" + self.htmlId


# --------------------------------------------------------------------
class File(Thing):
# --------------------------------------------------------------------
  """
  A FILE instance represents a HTML page, stylesheet, or other data
  file used by the website.
  
  A FILE instance may either contain some data or a pointer to some
  external resource. For instance, a FILE instance may represent a
  page of the website, or an anchor to an external page.

pages of the website can either
  contain actual HTML code (if generated), or simply a website page may be generated by
  this program from data read from disk, or just reference to an
  externally provided page. A File which does not hold data uses only
  self.href to store a pointer to the external data. For a file that
  holds data, baseName provides a name for the file that will be
  eventually produced. Also, dirName optionally provide a
  sub-directory name, if that file must generate a sub-directory.
  
  Files have a hierarchical structure which is used to construct
  indexes and to create the final directory layout of the produced
  website.
  """
  
  SITE = 0
  PAGE = 1
  STYLESHEET = 2
  SCRIPT = 3

  def __init__(self, type, parent=None, id=None):
    Thing.__init__(self, id)
    self.parent    = parent
    self.children  = []
    self.depth     = 0
    self.fileType  = type
    self.dirName   = None
    self.baseName  = None
    self.data      = None
    self.href      = None  
    self.title     = 'untitled'
    self.hide      = False
    if parent: 
      parent.addChild(self)
      self.depth = parent.depth + 1
    
  def __str__(self):
    return "%s type:%d path:%s" % (Thing.__str__(self), self.fileType, 
                                   self.getPathFromRoot())

  def addChild(self, file):
    """
    F.addChild(G) adds file G as a child of file F. Both the parent
    of G and the children of F are modified to relfect the addition.
    """
    self.children.append(file)
    file.parent = self

  def getPathFromRoot(self, dironly=False):
    """
    F.getPathFromRoot() compute the path of the file F in the output 
    hierarchy. The path is a chain of sub-directories associated
    to parent files, and the base name of F.
    
    F.getPathFromRoot(dironly=True) returns only the directory part
    of the path.
    """
    
    path = ""
    if self.href: return self.href
    if self.parent: path = self.parent.getPathFromRoot(dironly=True)
    if self.dirName: path += self.dirName + "/"
    if not dironly and self.baseName: path += self.baseName
    return path

  def getPathFrom(self, basedir, dironly=False):
    """
    F.getPathFrom(BASEDIR, DIRONLY) behaves as F.getPathFromRoot(DIRONLY),
    but the path is computed relatively to BASEDIR.
    """
    path = self.getPathFromRoot(dironly)
    c = 0
    while c < len(path) and c < len(basedir) and path[c] == basedir[c]:
      c = c + 1
    return re.sub(r'\w*/','../',basedir[c:]) + path[c:]


# --------------------------------------------------------------------
def iterateFiles(parentPage):
# --------------------------------------------------------------------
  """
  iterateFiles(parentPage) generates a depth first visit of the tree
  of pages rooted at parentPage.
  """
  for p in parentPage.children:
    yield p
    for q in iterateFiles(p): yield q

# --------------------------------------------------------------------
def findStyles(file):
# --------------------------------------------------------------------
  """
  A = findStyles(FILE) returns a list of stylesheets applicable to
  file FILE. These are all stylesheets that are children of FILE 
  or of any of its ancestors.
  """  
  styles = []
  if file.parent: styles = findStyles(file.parent)
  for x in file.children:
    if x.fileType == File.STYLESHEET:
      styles.append(x)
  return styles

# --------------------------------------------------------------------
class WebSite:
# --------------------------------------------------------------------
  """
  WebSite represent the whole website. A website is mainly a
  hierarchical collection of pages.
  """
  
  def __init__(self):
    self.root       = File(File.SITE)
    self.template   = template
    self.src        = ""
  
  def genFullName(self, name):
    """
    site.genFullName(NAME) generates the path to the file NAME,
    assumed relative to the XML file defining the website.
    """
    if name == None: return None
    if (os.path.isabs(name)):
      return name
    else:
      return os.path.join(os.path.dirname(self.src), name)
        
  def load(self, fileName):
    """
    Parse the XML document fileName.
    """
    self.src = fileName
    if verb:
      print "webdoc: parsing `%s'" % self.src
    doc = minidom.parse(self.src).documentElement
    self.xLoad(doc, self.root)

  def xLoad(self, doc, parent):
    """
    W.xLoad(DOC, PARENT) parses recursively the XML element DOC,
    appending the results as childern of PARENT.
    """
    for e in iterateChildNodes(doc):
      file = None

      if   e.tagName == 'stylesheet': fileType = File.STYLESHEET
      elif e.tagName == 'script':     fileType = File.SCRIPT
      elif e.tagName == 'page':       fileType = File.PAGE

      if e.tagName == 'stylesheet' or \
         e.tagName == 'page'       or \
         e.tagName == 'script':
        theId      = getAttribute(e, 'id')
        file       = File(fileType, parent, theId)
        file.src   = self.genFullName(getAttribute(e, 'src'))
        file.href  = getAttribute(e, 'href')
        file.title = getAttribute(e, 'title')
        file.hide  = getAttribute(e, 'hide') == 'yes'
        if file.src: 
          file.baseName = os.path.basename(file.src)
        
      if e.tagName == 'template':
        fileName = self.genFullName(e.getAttribute('src'))
        if verb:
          print "webdoc: setting template to `%s'" % fileName
        self.template = readText(fileName)

      # load children
      if e.tagName == 'page':
        self.xLoad(e, file)
        if len(file.children) > 0:
          file.dirName = os.path.splitext(file.baseName)[0]
          if file.dirName == 'index':
            file.dirName = None

      # extract refs
      if e.tagName == 'page' and file.src:
        file.data = readText(file.src)
        extractor = RefExtractor()
        extractor.feed(file.data)
        for i in extractor.ids:
          HtmlElement(file, i, i)
          
      # handle include 
      if e.tagName == 'include':
        if verb:
          src = self.genFullName(getAttribute(e, 'src'))
          print "webdoc: including `%s'\n" % src
          site = WebSite()
          try:
            site.load(src)
          except IOError, e:
            print "Could not access file %s (%s)" % (filename, e)
          except ExpatError, e:
            print "Error parsing file %s (%s)" % (filename, e)
          for x in iterateFiles(site.root):
            x.depth = x.depth + parent.depth + 1
          for x in site.root.children:
            parent.addChild(x)

  def genHtmlIndex(self, parentPage):
    """
    W.genHtmlIndex(PARENT) generates the HTML index starting from
    pate PARENT and down, recusrively.
    """
    html = ""
    hasContent = False
    if len(parentPage.children) == 0: return html
    parentIndent = " " * parentPage.depth
    html += parentIndent + "<ul>\n"
    for page in parentPage.children:
      if not (page.fileType == File.SITE or page.fileType == File.PAGE): continue
      if page.hide: continue
      indent = " " * page.depth
      html += indent + "<li>\n"
      html += indent + "<a href='%s'>%s</a>\n" % (page.id,page.title) 
      html += self.genHtmlIndex(page)
      html += indent + "</li>\n"
      hasContent = True
    html += parentIndent + "</ul>\n"
    if not hasContent: return ""
    return html
        
  def genSite(self):
    """
    W.genSite() generate the website, creating the file hierechiy
    """
    for file in iterateFiles(self.root):
    
      dirName = file.getPathFromRoot(True)
      fileName = os.path.join(outdir, file.getPathFromRoot())

      if file.fileType == File.STYLESHEET:
        print "webdoc: copying stylesheet `%s' from `%s'" % (fileName, file.src)
        writeText(fileName, readText(file.src))
        
      if file.fileType == File.SCRIPT:
        print "webdoc: copying script `%s' from `%s'" % (fileName, file.src)
        writeText(fileName, readText(file.src))
      
      if file.fileType == File.PAGE:
        page = file
        if page.data:
          text = self.template
          rootpath = self.root.getPathFrom(dirName, True)

          # stylesheets block
          block = ""
          for x in findStyles(page):
            block += '<link rel="stylesheet" href="%s" type="text/css"/>\n' % \
                x.getPathFrom(dirName)
                       
          text = re.sub("%stylesheet;", block, text)
          text = re.sub("%pagetitle;", "VLFeat - %s" % page.title, text)
          text = re.sub("%title;", "<h1>VLFeat.org</h1>", text)
          text = re.sub("%subtitle;", "<h2>%s</h2>" % page.title, text)
          text = re.sub("%index;", self.genHtmlIndex(self.root), text)
          text = re.sub("%content;", page.data, text)
          text = re.sub("%root;", rootpath, text)
          
          generator = PageGenerator(site, dirName)
          generator.feed(text)
          text = generator.output()

          writeText(fileName, text)
          print "webdoc: wrote page %s" % (fileName)
        
  def debug(self):
    print "=== Index ==="
    print self.genHtmlIndex(self.root)
    print "=== IDs ==="
    Thing.dumpDirectory()


# --------------------------------------------------------------------    
class RefExtractor(HTMLParser):
# --------------------------------------------------------------------
  def __init__(self):
    HTMLParser.__init__(self)
    self.ids = []

  def handle_starttag(self, tag, attrs):
    for (k,i) in attrs:
      if k == 'id':
        self.ids.append(i)
        
# --------------------------------------------------------------------    
class PageGenerator(HTMLParser):
# --------------------------------------------------------------------
  def __init__(self, site, basedir):
    HTMLParser.__init__(self)
    self.pieces = []
    self.site = site
    self.basedir = basedir
    
  def handle_starttag(self, tag, attrs):
    for n in range(len(attrs)):
      (k,i) = attrs[n]
      if k == 'href':
        if i in Thing.directory:
          path = ""
          thing = Thing.directory [i]
          path = thing.getPathFrom(self.basedir)
          attrs[n] = (k, path)
    strattrs = "".join([' %s="%s"' % (key, value) for 
      key, value in attrs])
    self.pieces.append("<%(tag)s%(strattrs)s>" % locals())

  def handle_endtag(self, tag):
    self.pieces.append("</%(tag)s>" % locals())
    
  def handle_charref(self, ref):         
    self.pieces.append("&#%(ref)s;" % locals())
    
  def handle_entityref(self, ref):       
    self.pieces.append("&%(ref)s" % locals())
    if htmlentitydefs.entitydefs.has_key(ref):
        self.pieces.append(";")

  def handle_data(self, text):           
    self.pieces.append(text)

  def handle_comment(self, text):        
    self.pieces.append("<!--%(text)s-->" % locals())

  def handle_pi(self, text):             
    self.pieces.append("<?%(text)s>" % locals())

  def handle_decl(self, text):
    self.pieces.append("<!%(text)s>" % locals())

  def output(self):              
    """Return processed HTML as a single string"""
    return "".join(self.pieces)

# --------------------------------------------------------------------    
if __name__ == '__main__':
# --------------------------------------------------------------------

  (options, args) = parser.parse_args()
  filename = args[0]
  
  verb = options.verb
  xmldoc = args[0]
  outdir = options.outdir

  if verb:
    print "webdoc: website document: `%s'" % xmldoc
    print "webdoc: output path: `%s'" % outdir

  site = WebSite()
  site.load(xmldoc)
  site.debug()
  site.genSite()

