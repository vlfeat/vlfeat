#!/usr/bin/python

import sys,os

def usage():
    print "Usage:", sys.argv[0], "<wikiname> <htmlname>"
    print ""
    print "Replaces wiki documentation with documentation from html files"

def wikidoc(pagename, htmlfile):
    f = open(htmlfile)
    doclines = f.readlines()
    f.close()

    starttag = "<!-- WIKIDOC START -->"
    endtag   = "<!-- WIKIDOC END -->"
    doclines.insert(0, starttag + '\n')
    doclines.append('\n' + endtag + '\n')

    wikilines = []
    if os.path.exists(pagename):
        f = open(pagename)
        wikilines = f.readlines()
        f.close()

    startline = -1
    endline = -1
    for i in range(len(wikilines)):
        if wikilines[i].strip() == starttag and startline == -1: startline = i
        if wikilines[i].strip() == endtag   and endline == -1:   endline = i

    if startline == -1 or endline == -1 or startline >= endline:
        print "Wikidoc warning: Didn't find tags for", pagename
        startline = 0
        endline = 0

    wikilines = wikilines[:startline] + doclines + wikilines[endline+1:]
    f = open(pagename, 'w')
    f.writelines(wikilines)
    f.close()

if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage()
        sys.exit(2)

    print "wikidoc: using", sys.argv[1], "for page", sys.argv[2]
    wikidoc(sys.argv[1], sys.argv[2])
