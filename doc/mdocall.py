#!/usr/bin/python

import sys,os

from wikidoc import wikidoc
from m2ascii import convertfile
from formatter import Formatter

def usage():
    print "Usage:", sys.argv[0], "<basedir> <docdir>"
    print ""
    print "Takes all .m files in basedir and its subdirectories and converts"
    print "them to html documentation, placing the results in docdir."

import subprocess, signal
def runcmd(cmd):
    try:
        p = subprocess.Popen(cmd, shell=True)
        sts = os.waitpid(p.pid,0)
    except (KeyboardInterrupt, SystemExit):
        os.kill(p.pid, signal.SIGKILL)
        raise


def getmfiles(basedir, docdir):
    mfiles = {}
    for root, dirs, files in os.walk(basedir):
        for name in files:
            if name.endswith('.m'):
                mname = os.path.join(root,name)

                (basename, ext) = os.path.splitext(mname)
                basename = basename[len(basedir):]
                funcname = os.path.basename(basename)

                htmlname = os.path.join(docdir, basename + '.html')
                #parts = basename.split(os.path.sep)
                #parts = [p.capitalize() for p in parts]
                #linkname = "Doc" + os.path.basename(basename).capitalize()
                linkname = "Doc_" + basename.replace(os.path.sep, '_')

                if not os.path.isdir(os.path.dirname(htmlname)):
                    os.makedirs(os.path.dirname(htmlname))

                if funcname in mfiles:
                    print "Warning: Duplicate function name:", funcname
                    print "  ", mfiles[funcname][0], "-", mname

                mfiles[funcname] = (mname, htmlname, linkname)

    return mfiles

def overviewpage(pages, mfiles, indent):
    keys = pages.keys()
    keys.sort()
    pagelines = []
    for k in keys:
        if pages[k] == 0:
            (mname, htmlname, linkname, shortdesc) = mfiles[k]
            if indent == "":
                pagelines.append("[[%s|%s]] %s<br>\n" % (linkname, k, shortdesc))
            else:
                pagelines.append("%s [[%s|%s]] %s\n" % (indent, linkname, k,
                    shortdesc))

    for k in keys:
        if pages[k] != 0:
            if indent == "":
                pagelines.append("=== %s ===\n" % (k))
            else:
                pagelines.append(indent + k + ":\n")
            pagelines.extend(overviewpage(pages[k], mfiles, indent + '*'))
    return pagelines

if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage()
        sys.exit(2)
    
    basedir = sys.argv[1]
    # Add trailing slash
    if not basedir.endswith('/'): basedir = basedir + "/"
    docdir  = sys.argv[2]

    # build a dictionary of links for ascii doc
    mfiles = getmfiles(basedir, docdir)
    linkdict = {}
    for funcname in mfiles:
        (mname, htmlname, linkname) = mfiles[funcname]
        #linkdict[funcname.upper()] = "[[%s|%s]]" % (linkname, file)
        linkdict[funcname.upper()] = "%s" % (linkname)

    # convert to txt, then run ascii doc
    for funcname in mfiles:
        (mname, htmlname, linkname) = mfiles[funcname]
        (txtname, ext) = os.path.splitext(htmlname)
        txtname += '.txt'
        print mname, txtname
        shortdesc = convertfile(mname, txtname)
        mfiles[funcname] += (shortdesc,)
        f = open(txtname)
        lines = f.readlines()
        f.close()
       
        content = ''
        if len(lines) > 0:
            formatter = Formatter(lines, linkdict, 'wiki')

            content = formatter.toDOM().toxml("UTF-8")
            contentlines = content.splitlines()
            contentlines = contentlines[1:]
            content = '\n'.join(contentlines)
        
        f = open(htmlname, 'w')
        f.write(content)
        f.close()
        os.remove(txtname)

    # make summary page
    pagegroups = {}
    for funcname in mfiles:
        (mname, htmlname, linkname, shortdesc) = mfiles[funcname]
        patharray = htmlname[len(docdir):].split(os.path.sep)

        path = pagegroups
        # walk the path and create a dictionary list of files
        for i in range(len(patharray)-1):
            p = patharray[i]
            if not p in path:
                path[p] = {}
            path = path[p]
        path[funcname] = 0

    overviewname = os.path.join(docdir, "index.html")
    pagelines = overviewpage(pagegroups, mfiles, '')
    pagelines.insert(0, "== Documentation ==\n")
    wikiname = "Doc_index.wiki"
    f = open(overviewname, 'w')
    f.writelines(pagelines)
    f.close()

    try:
        runcmd("mvs update %s" % wikiname)
        print "Converting", overviewname, "to", wikiname
        wikidoc(wikiname, overviewname)
        runcmd("mvs commit -M -m 'Documentation update' %s" % wikiname)
    except (KeyboardInterrupt, SystemExit):
        sys.exit(1)

    # checkin files to wiki
    for funcname in mfiles:
        (mname, htmlname, linkname, shortdesc) = mfiles[funcname]
        wikiname = linkname + ".wiki"
        try:
            runcmd("mvs update %s" % wikiname)
            print "Converting", htmlname, "to", wikiname
            wikidoc(wikiname, htmlname)
            runcmd("mvs commit -M -m 'Documentation update' %s" % wikiname)
            # possibly add -M for minor revision
        except (KeyboardInterrupt, SystemExit):
            sys.exit(1)
