#!/usr/bin/python

import sys

def usage():
    print "Usage:", sys.argv[0], "<mfile> <asciifile>"
    print ""
    print "Extracts function documentation from a Matlab .m file and"
    print "converts the file to an asciidoc compatible text file."

def m2ascii(mfile):
    ascii = []
    header = ""
    seenfunction = False
    seenpercent  = False
    for l in open(mfile):
        line = l.strip().lstrip() # remove whitespace and newline
        if line.startswith('%'): seenpercent = True
        if line.startswith('function'): 
            seenfunction = True
            continue

        if not line.startswith('%'):
            if (seenfunction and seenpercent) or not seenfunction:
                break
            else:
                continue
        line = line[1:] # Remove leading %
        ascii.append('%s\n' % line)
    if len(ascii) > 0:
        header = ascii[0].strip()
        ascii = ascii[1:]
    return (ascii, header)

def convertfile(mfile, afile):
    f = open(afile, 'w')
    (ascii, header) = m2ascii(mfile)
    f.writelines(ascii)
    f.close()
    return header

if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage()
        sys.exit(2)

    mfile = sys.argv[1]
    afile = sys.argv[2]
    print "m2ascii:", mfile, "=>", afile
    convertfile(mfile, afile)


