#!/usr/bin/python
#
#  webdoc.py
#  vlfeat
#

# AUTORIGHTS

import sys, os, re, subprocess, signal, htmlentitydefs, shutil
from wikidoc import wikidoc
from optparse import OptionParser
from HTMLParser import HTMLParser
from xml.parsers.expat import ExpatError
from xml.dom import minidom 

usage = """usage: %prog [options] WEB.XML <docdir>

"""

template = """

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
    help    = "directory containing the produced HTML files")

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
    print "Warning! Could not open text file '%s'" % fileName
  return text
  
# --------------------------------------------------------------------
def writeText(fileName, text):
# --------------------------------------------------------------------
  """
  writeText(NAME, TEXT) writes TEXT to the file NAME.
  """
  try:
    file = open(fileName, 'w')
    file.write(text)
    file.close()
  except IOError:
    print "Warning! Could not write text file '%s'" % fileName

# --------------------------------------------------------------------
def iterateChildNodesByTag(node, tag):
# --------------------------------------------------------------------
  """
  This generator searches the childern of NODE for 
  XML elements matching TAG.
  """
  n = node.firstChild
  while n:
    if n.nodeType == n.ELEMENT_NODE and n.nodeName == tag:
      yield n
    n = n.nextSibling
    
# --------------------------------------------------------------------
def getAttribute(element, attr, default=None):
# --------------------------------------------------------------------
  if element.hasAttribute(attr):
    return element.getAttribute(attr)
  else:
    return default
    
# --------------------------------------------------------------------
class Page:
# --------------------------------------------------------------------
  def __init__(self):
    self.text = None
    self.href = None
    self.title = 'Untitled'
    self.src = None
    self.depth = 0
    self.children = []

# --------------------------------------------------------------------
def iteratePages(parentPage):
# --------------------------------------------------------------------
  """
  Iterate over a tree of pages in depth first order.
  """
  for p in parentPage.children:
    yield p
    for q in iteratePages(p): yield q

# --------------------------------------------------------------------
class ID:
# --------------------------------------------------------------------
  """
  A class instance represents an element ID which has a validity 
  throughout the website.
  """

  def __init__(self, page, htmlId=None):
    self.page = page
    self.htmlId = htmlId
    
  def __str__(self):
    return "%s (%s)" % (self.page.src, self.htmlId)

# --------------------------------------------------------------------
class WebSite:
# --------------------------------------------------------------------
  """
  WebSite represent the whole website. A website is mainly a
  hierarchical collection of pages.
  """
  
  def __init__(self):
    self.root = Page()
    self.ids = {}
    self.stylesheet = ""
    self.template   = ""
    self.src        = ""
  
  def getFullName(self, name):
    if name == None: return None
    if (os.path.isabs(name)):
      return name
    else:
      return os.path.join(os.path.dirname(self.src), name)
    
  def load(self, fileName):
    if verb:
      print "webdoc: loading website `%s'\n" % fileName

    self.src = fileName
    doc = minidom.parse(self.src).documentElement
    for e in iterateChildNodesByTag(doc, 'template'):
      self.template = self.getFullName(e.getAttribute('src'))
    for e in iterateChildNodesByTag(doc, 'stylesheet'):
      self.stylesheet = self.getFullName(e.getAttribute('src'))
    self.root.children = self.xLoadPages(doc, 1)
    self.ids = self.extractRefs(self.root)

  def xLoadPages(self, doc, depth):
    pages = []
    for e in iterateChildNodesByTag(doc, 'page'):
      page          = Page()
      page.id       = getAttribute(e, 'id')
      page.href     = getAttribute(e, 'href')
      page.src      = self.getFullName(getAttribute(e, 'src'))
      page.title    = getAttribute(e, 'title')
      page.depth    = depth
      page.hide     = getAttribute(e, 'hide')
      if page.src is not None:
        page.text = readText(page.src)
      if page.href is None:
        page.href = os.path.basename(page.src)
      else:
        page.href = page.href
      page.children = self.xLoadPages(e, depth + 1)
      pages.append(page)

    for i in iterateChildNodesByTag(doc, 'include'):
      filename = self.getFullName(getAttribute(i, 'src'))
      if verb:
        print "webdoc: including website `%s'\n" % filename
      site = WebSite()
      try:
        site.load(filename)
        pages[len(pages):] = site.root.children

      except IOError, e:
        print "Could not access file %s (%s)" % (filename, e)
      except ExpatError, e:
        print "Error parsing file %s (%s)" % (filename, e)
    return pages

  def genHtmlIndex(self, parentPage, activePage = None):
    html = ""
    if len(parentPage.children) == 0: return html
    parentIndent = " " * parentPage.depth
    html += parentIndent + "<ul>\n"
    for page in parentPage.children:
      if page.hide == 'yes': continue
      indent = " " * page.depth
      html += indent + "<li>\n"
      html += indent + "<a href='%s'>%s</a>\n" % (page.id,page.title) 
      html += self.genHtmlIndex(page, activePage)
      html += indent + "</li>\n"
    html += parentIndent + "</ul>\n"
    return html
    
  def extractRefs(self, parentPage):
    ids = {}
    for page in iteratePages(self.root):
      print "extra page %s (id: %s)" % (page.title, page.id)
      ids[page.id] = ID(page)
      if page.text is not None:
        extractor = RefExtractor()
        extractor.feed(page.text)
        for i in extractor.ids:
          ids[i] = ID(page, i)
    return ids
    
  def genSite(self):
    print "copying stylesheet %s to %s" % (
      self.stylesheet, 
      os.path.join(outdir, os.path.basename(self.stylesheet)))
    shutil.copy(
      self.stylesheet,
      os.path.join(outdir, os.path.basename(self.stylesheet)))
    for page in iteratePages(self.root):
      if page.text is None: continue
      text = readText(os.path.join(srcdir, self.template))
      text = re.sub("%stylesheet;", os.path.basename(self.stylesheet), text)
      text = re.sub("%pagetitle;", "VLFeat - %s" % page.title, text)
      text = re.sub("%title;", "<h1>VLFeat</h1>", text)
      text = re.sub("%subtitle;", "<h2>%s</h2>" % page.title, text)
      text = re.sub("%index;", self.genHtmlIndex(self.root), text)
      text = re.sub("%content;", page.text, text)
      generator = PageGenerator(site)
      generator.feed(text)
      outName = os.path.join(outdir, os.path.basename(page.src))
      writeText(
        outName,
        generator.output())
      print "webdoc: wrote %s" % (outName)
        
  def debug(self):
    print "=== Index ==="
    print self.genHtmlIndex(self.root)
    print "=== IDs ==="
    for (k,i) in self.ids.iteritems():
      print "%30s) %s" % (k, i)

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
  def __init__(self, site):
    HTMLParser.__init__(self)
    self.pieces = []
    self.site = site
    
  def handle_starttag(self, tag, attrs):
    for n in range(len(attrs)):
      (k,i) = attrs[n]
      if k == 'href':
        if i in self.site.ids:
          id = self.site.ids[i]
          if id.htmlId is None:
            attrs[n] = (k, id.page.href)
          else:
            attrs[n] = (k, "%s#%s" % (id.page.href, id.htmlId))  
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

  #
  # Parse comand line options
  #
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
  
  
  
    
    


