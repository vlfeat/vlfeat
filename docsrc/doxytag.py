#!/bin/python
# file: Doxytag.py
# brief: Parse doxygen.tag file
# author: Andrea Vedaldi

"""
DOXYTAG parses doxygen.tag files.
"""

import xml.dom.minidom
import sys
import os
import re

__mpname__           = 'Doxytag'
__version__          = '0.1'
__date__             = '2013-07-09'
__description__      = 'Doxygen tags parser'
__long_description__ = __doc__
__license__          = 'BSD'
__author__           = 'Andrea Vedaldi'

# --------------------------------------------------------------------
class Doxytag:
# --------------------------------------------------------------------
    """
    d = Doxytag(FILENAME)
    """
    def __init__ (self, filename):
        self.index = {}
        dom = xml.dom.minidom.parse(filename)
        self.xBuildIndex(dom)

    def getText(self, nodelist):
        rc = []
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                rc.append(node.data)
        return ''.join(rc)

    def xBuildIndex(self, node):
        if node.nodeType == node.ELEMENT_NODE:
            if node.tagName == "docanchor":
                anchor=self.getText(node.childNodes)
                filename=node.getAttribute("file")
                if os.path.splitext(filename)[1] != '.html':
                        filename += '.html'
                self.index[anchor] = filename + "#" + anchor
            # do filenames
            filename = node.getElementsByTagName("filename")
            if filename.length > 0:
                name = node.getElementsByTagName("name")
                if name.length > 0:
                    s1 = self.getText(name[0].childNodes)
                    s2 = self.getText(filename[0].childNodes)
                    if os.path.splitext(s2)[1] != '.html':
                        s2 = s2 + '.html'
                    self.index[s1] = s2

        for x in node.childNodes:
            self.xBuildIndex(x)

    def dump(self):
        for k in self.index:
            print "%40s -> %40s" % (k, self.index[k])

if __name__ == '__main__':
    d = Doxytag("doc/doxygen.tag")
    d.dump()
