#!/usr/bin/python
# file:        webdoc.py
# author:      Andrea Vedaldien
# description: Implementation of webdoc.

# AUTORIGHTS
# Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
#
# This file is part of VLFeat, available in the terms of the GNU
# General Public License version 2.

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

usage = """webdoc [OPTIONS...] <DOC.XML>

--outdir   Set output directory
--verbose  Be verbose
"""

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
    default = "html",
    action  = "store",
    help    = "write output to this directory")

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

def getUniqueNodeID(id = None):
    """
    getUniqueNodeID() generates an unique ID for a document node.
    getUniqueNodeID(id) generates an unique ID adding a suffix to id.
    """
    if id is None: id = "id"
    uniqueId = id
    count = 0
    while 1:
        if uniqueId not in nodeIndex: break
        count += 1
        uniqueId = "%s-%d" % (id, count)
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

def walkNodes(rootNode, nodeType = None):
    for n in rootNode.getChildren():
        for m in walkNodes(n, nodeType):
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
class DocBareParsingError(Exception):
# --------------------------------------------------------------------
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return self.message

# --------------------------------------------------------------------
class DocParsingError(Exception):
# --------------------------------------------------------------------
    def __init__(self, URL, row, column, message):
        self.URL = URL
        self.rowNumber = row
        self.columnNumber = column
        self.message = message

    def __str__(self):
        return "%s:%d:%d:%s" % (self.URL,
                                self.rowNumber,
                                self.columnNumber,
                                self.message)

# --------------------------------------------------------------------
# Better error reporting
# --------------------------------------------------------------------

class makeBareGuard(object):
    def __init__(self, func):
        self.func = func

    def __call__(self, obj, *args, **keys):
        try:
            self.func(obj, *args, **keys)
        except DocParsingError:
            raise
        except DocBareParsingError:
            raise
        except Exception, e:
            raise obj.makeBareParsingError(e.__str__())

    def __get__(self, obj, type=None):
        return types.MethodType(self, obj, type)

class makeGuard(object):
    def __init__(self, func):
        self.func = func

    def __call__(self, obj, *args, **keys):
        try:
            self.func(obj, *args, **keys)
        except DocParsingError:
            raise
        except DocBareParsingError, e:
            raise obj.makeParsingError(e.message)
        except Exception, e:
            raise obj.makeParsingError(e.__str__())

    def __get__(self, obj, type=None):
        return types.MethodType(self, obj, type)

# --------------------------------------------------------------------
class DocBareNode:
# --------------------------------------------------------------------
    """
    A node of the document tree without parent, childnren, or any
    other attribute. It is used to implement frequent leaf nodes such
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
    def publish(self, generator, pageNode = None): pass
    def publishIndex(self, gen, pageNode, openNodeStack): return False

    def makeBareParsingError(self, message):
        """
        Creates a DocParsingError with the specified message.  The
        source code URL, row, and column are set to the node
        corresponding values.
        """
        return DocBareParsingError(message)

# --------------------------------------------------------------------
class DocNode(DocBareNode):
# --------------------------------------------------------------------
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
        if URL:
            if not URL is self.getSourceURL():
                self.sourceURL = URL
        if locator:
            self.sourceRow = locator.getLineNumber()
            self.sourceColumn = locator.getColumnNumber()
        nodeIndex[self.id] = self

    def __str__(self):
        return "%s:%d:%d:%s" % (self.getSourceURL(),
                                self.getSourceRow(),
                                self.getSourceColumn(),
                                self.getID())

    def dump(self):
        """
        Recusively dump the tree, for debugging purposes.
        """
        depth = self.getDepth()
        print " " * depth, self
        for x in self.children: x.dump()



    def getID(self):
        """
        Returns the node ID
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

    def getSourceURL(self):
        """
        Get the URL of the source code file where the node was
        instantiated.
        """
        if self.sourceURL:
            return self.sourceURL
        elif self.parent:
            return self.parent.getSourceURL()
        else:
            return ""

    def getSourceColumn(self):
        """
        Gets the column of the source code file where the node was
        instantiated.
        """
        if not self.sourceColumn is None:
            return self.sourceColumn
        elif self.parent:
            return self.parent.getSourceColumn()
        else:
            return -1

    def getSourceRow(self):
        """
        Gets the row (line) of the source code file where the node was
        instantiated.
        """
        if not self.sourceRow is None:
            return self.sourceRow
        elif self.parent:
            return self.parent.getSourceRow()
        else:
            return -1

    def makeParsingError(self, message):
        """
        Creates a DocParsingError with the specified message.  The
        source code URL, row, and column are set to the node
        corresponding values.
        """
        return DocParsingError(
            self.getSourceURL(),
            self.getSourceRow(),
            self.getSourceColumn(),
            message)

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

    def publish(self, generator, pageNode = None):
        """
        Recursively calls PUBLISH() on its children.
        """
        for c in self.getChildren():
            c.publish(generator, pageNode)
        return None

    def publishIndex(self, gen, pageNode, openNodeStack):
        """
        Recursively calls PUBLISHINDEX() on its children.
        """
        hasIndexedChildren = False
        for c in self.getChildren():
            hasIndexedChildren = c.publishIndex(gen, pageNode, openNodeStack) \
                or hasIndexedChildren
        return hasIndexedChildren

# --------------------------------------------------------------------
def expandAttr(value, pageNode):
# --------------------------------------------------------------------
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
                print "warning: could not cross-reference '%s'" % toNodeID
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
                print "warning: environment variable '%s' not defined" % envName
            continue
        raise DocBareParsingError(
            "Unknown directive '%s' found while expanding attribute" % directive)
    if next < len(value): xvalue += value[next:]
    #print "EXPAND: ", value, " -> ", xvalue
    return xvalue

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
        fid.write(str.encode('latin-1'))

    def putXMLString(self, str):
        fid = self.fileStack[-1]
        xstr = xml.sax.saxutils.escape(str, mapUnicodeToHtmlEntity)
        try:
            fid.write(xstr.encode('latin-1'))
        except:
            print "OFFENDING", str, xstr
            print mapUnicodeToHtmlEntity[str]
            raise

    def putXMLAttr(self, str):
        fid = self.fileStack[-1]
        xstr = xml.sax.saxutils.quoteattr(str)
        fid.write(xstr.encode('latin-1'))

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
class DocInclude(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)
        if not attrs.has_key("src"):
            raise self.makeParsingError("include missing 'src' attribute")
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
            raise self.makeParsingError("dir tag missing 'name' attribute")
        self.dirName = attrs["name"]

    def __str__(self):
        return DocNode.__str__(self) + ":<web:dir name=%s>" \
            % xml.sax.saxutils.quoteattr(self.dirName)

    def getPublishDirName(self):
        return self.parent.getPublishDirName() + self.dirName + os.sep

    def publish(self, generator, pageNode = None):
        generator.changeDir(self.dirName)
        DocNode.publish(self, generator, pageNode)
        generator.parentDir()

    publish = makeGuard(publish)

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
        if pageNode is None: return
        if not pageNode: return
        gen.putString(self.text)

    publish = makeBareGuard(publish)

# --------------------------------------------------------------------
class DocCDATA(DocNode):
# --------------------------------------------------------------------
    def __init__(self):
        DocNode.__init__(self, {}, None, None)

    def __str__(self):
        return DocNode.__str__(self) + ":CDATA"

    def publish(self, gen, pageNode = None):
        if pageNode is None: return
        gen.putString("<![CDATA[")
        DocNode.publish(self, gen, pageNode)
        gen.putString("]]>") ;

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
        if pageNode is None: return
        # find occurences of %directive; in the text node and do the
        # appropriate substitutions
        next = 0
        for m in re.finditer("%(\w+)(:.*)?;", self.text):
            if next < m.start():
                gen.putXMLString(self.text[next : m.start()])
            next = m.end()
            directive = self.text[m.start()+1 : m.end()-1]
            directive = m.group(1)

            if   directive == "content":
                pageNode.publish(gen, pageNode)

            elif directive == "pagestyle":
                for s in pageNode.findChildren(DocPageStyle):
                    s.publish(gen, pageNode)

            elif directive == "pagescript":
                for s in pageNode.findChildren(DocPageScript):
                    s.publish(gen, pageNode)

            elif directive == "pagetitle":
                gen.putString(pageNode.title)

            elif directive == "path":
                ancPages = [x for x in walkAncestors(pageNode, DocPage)]
                ancPages.reverse()
                gen.putString(" - ".join([x.title for x in ancPages]))

            elif directive == "navigation":
                gen.putString("<ul>\n")
                openNodeStack = [x for x in walkAncestors(pageNode, DocPage)]
                siteNode = walkAncestors(pageNode, DocSite).next()
                siteNode.publishIndex(gen, pageNode, openNodeStack)
                gen.putString("</ul>\n")

            elif directive == "env":
                envName = m.group(2)[1:]
                if envName in os.environ:
                    gen.putString(os.environ[envName])
                else:
                    print "warning: environment variable '%s' not defined" % envName
            else:
                print "warning: ignoring unknown directive " + label
        if next < len(self.text):
            gen.putXMLString(self.text[next:])

    publish = makeBareGuard(publish)

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
        if pageNode is None: return
        gen.putString("<")
        gen.putString(self.tag)
        for name, value in self.attrs.items():
            gen.putString(" ")
            gen.putString(name)
            gen.putString("=")
            gen.putXMLAttr(expandAttr(value, pageNode))
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

    def publish(self, generator, pageNode = None):
        if pageNode is None: return
        DocNode.publish(self, generator, pageNode)

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocPageStyle(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

    def publish(self, gen, pageNode = None):
        if pageNode is None: return
        sa = self.getAttributes()
        gen.putString("<style rel=\"stylesheet\" type=")
        if sa.has_key("type"):
            gen.putXMLAttr(expandAttr(sa["type"], pageNode))
        else:
            gen.putString("\"text/css\" ")
        if sa.has_key("href"):
            gen.putString("href=")
            gen.putXMLAttr(expandAttr(sa["href"], pageNode))
        gen.putString(">")
        DocNode.publish(self, gen, pageNode)
        gen.putString("</style>")

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocPageScript(DocNode):
# --------------------------------------------------------------------
    def __init__(self, attrs, URL, locator):
        DocNode.__init__(self, attrs, URL, locator)

    def publish(self, gen, pageNode = None):
        if pageNode is None: return
        sa = self.getAttributes()
        gen.putString("<script type=")
        if sa.has_key("type"):
            gen.putXMLAttr(expandAttr(sa["type"], pageNode))
        else:
            gen.putString("\"text/javascript\" ")
        if sa.has_key("src"):
            gen.putString("src=")
            gen.putXMLAttr(expandAttr(sa["src"], pageNode))
        gen.putString(">")
        DocNode.publish(self, gen, pageNode)
        gen.putString("</script>")

    publish = makeGuard(publish)

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
                raise self.makeParsingError(
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

    def publish(self, generator, pageNode = None):
        if not pageNode:
            generator.open(self.getPublishFileName())
            templateNode = nodeIndex[self.templateID]
            templateNode.publish(generator, self)
            generator.close()
            DocNode.publish(self, generator, None)
        elif pageNode is self:
            DocNode.publish(self, generator, pageNode)

    def publishIndex(self, gen, pageNode, openNodeStack):
        if self.hide: return
        gen.putString("<li><a href=")
        gen.putXMLAttr(
            expandAttr("%%pathto:%s;" % self.getID(), pageNode))
        if len(openNodeStack) == 1 and self == openNodeStack[0]:
            gen.putString(" class='active' ")
        gen.putString(">")
        gen.putXMLString(self.title)
        gen.putString("</a>\n")
        pos = gen.tell()
        gen.putString("<ul>\n")
        hasIndexedChildren = False
        if len(openNodeStack) > 0 and self == openNodeStack[-1]:
            openNodeStack.pop()
            hasIndexedChildren = DocNode.publishIndex(self, gen, pageNode, openNodeStack)
        if hasIndexedChildren:
            gen.putString("</ul>")
        else:
            gen.seek(pos)
        gen.putString("</li>\n")
        return True

    publish = makeGuard(publish)

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

    def getPublishDirName(self):
        return ""

    def getOutDir(self):
        return self.outDir

    def setOutDir(self, outDir):
        self.outDir = outDir

    def publish(self):
        generator = Generator(self.outDir)
        DocNode.publish(self, generator)

    publish = makeGuard(publish)

# --------------------------------------------------------------------
class DocHandler(ContentHandler):
# --------------------------------------------------------------------

    def __init__(self):
        ContentHandler.__init__(self)
        self.rootNode = None
        self.stack = []
        self.locatorStack = []
        self.filePathStack = []
        self.verbosity = 1
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

    def startElement(self, name, attrs):
        # convert attrs to an actual dictionary
        attrs_ = {}
        for k, v in attrs.items():
            attrs_[k] = v
        attrs = attrs_

        URL = self.getCurrentFileName()
        locator = self.getCurrentLocator()

        if name == "include":
            if not attrs.has_key("src"):
                raise DocParsingError(
                    URL,
                    locator.getLineNumber(),
                    locator.getColumnNumber(),
                    "<web:include> missing 'src' attribute")
            filePath = attrs["src"]
            qualFilePath = self.lookupFile(filePath)
            if qualFilePath is None:
                raise DocParsingError(
                    URL,
                    locator.getLineNumber(),
                    locator.getColumnNumber(),
                    "<web:include> could not find file '%s'" % filePath)
            if self.verbosity > 0:
                print "parsing '%s'" % qualFilePath
            if attrs.has_key("type"):
                includeType = attrs["type"]
            else:
                includeType = "webdoc"
            if includeType == "webdoc":
                self.load(qualFilePath)
            elif includeType == "text":
                self.characters(open(qualFilePath, 'r').read())
            else:
                raise DocParsingError(
                    URL,
                    locator.getLineNumber(),
                    locator.getColumnNumber(),
                    "<web:include> type '%s' is not defined" % includeType)
            return

        if len(self.stack) == 0:
            parent = None
        else:
            parent = self.stack[-1]
        node = None

        if   name == "site":
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
        else:
            node = DocHtmlElement(name, attrs, URL, locator)

        if parent: parent.adopt(node)
        self.stack.append(node)

    def endElement(self, name):
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
        parser.parse(qualFilePath)

    def setDocumentLocator(self, locator):
        self.locatorStack.append(locator)

    def getCurrentLocator(self):
        if len(self.locatorStack) > 0:
            return self.locatorStack[-1]
        else:
            return None

    def characters(self, content):
        parent = self.stack[-1]
        if parent.isA(DocCDATA):
            node = DocCDATAText(content)
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
if __name__ == '__main__':
# --------------------------------------------------------------------
    (opts, args) = parser.parse_args()

    filePath = args[0]
    handler = DocHandler()
    try:
        handler.load(filePath)
    except (DocParsingError, xml.sax.SAXParseException), (e):
        print e
        sys.exit(-1)

    # configure
    handler.rootNode.setOutDir(opts.outdir)

    #print "== Index Content =="
    # dumpIndex()
    #print
    #print "== Node Tree =="
    #handler.rootNode.dump()

    print "== All pages =="
    for x in walkNodes(handler.rootNode, DocPage):
        print x

    print "== Publish =="
    try:
        handler.rootNode.publish()
    except (DocParsingError, xml.sax.SAXParseException), (e):
        print e
        sys.exit(-1)
    sys.exit(0)
