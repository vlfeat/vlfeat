#!/bin/bash
# file:        update-mak.sh
# description: Helper script to update Makefile.mak
# author:      Andrea Vedaldi

# AUTORIGHTS

# This script updates Makefile.mak based on the current source tree
# and version in vl/generic.h. It saves the new version to
# Makefile.mak.new.

function make
{
    # sed below removes the trailing '\' from the last entry
    (
        printf '%s \\\n' "$1 ="
        find "$2" \
            -name '*.c' \
            -not -path 'vl/doc/*' \
            |  tr '/' '\' | while read -r x
        do
            printf '  %s \\\n' "$x"
        done
    ) | sed '$s/ *\\$//'
    echo
}

function sub
{
    # Put substitution text at the beginning. Then sed puts the
    # beginning in its hold buffer, finds the paragraph starting with
    # "$2 =", deletes all but the last lines of it and substititue the
    # last line with the hold buffer. It also removes trailing
    # whitespaces.
    (make "$2" "$3" ; echo "$1") | \
        sed -e '1{h;d;}' \
            -e '2,/^$/{H;d;}' \
            -e "/$2 =/,/^$/{/^$/!d;g;}" \
            -e 's/ *$//'
}

function subv
{
    # Substitutes VER = x.x.x with the new version string.
    echo "$1" | sed -e "/VER *=/s/\(\([0-9][0-9]*\.\{0,1\}\)\{3\}\)/$2/"
}

# source version string from vl/generic.h
ver=$(cat vl/generic.h | sed -n \
    's/.*VL_VERSION_STRING.*\"\([0-9.]*\)\".*/\1/p')

# source current nmake script
a=$(cat Makefile.mak)

# updaetes version string
a=$(subv "$a" "$ver")

# updates hard-wired list of source code files in the nmake script
a=$(sub "$a" libsrc vl)
a=$(sub "$a" cmdsrc src)
a=$(sub "$a" mexsrc toolbox)

echo "$a" > Makefile.mak.new

diffs=$(diff Makefile.mak Makefile.mak.new)
if [ -z "$diffs" ]
then
    echo "Makefile.mak is up to date"
    rm Makefile.mak.new
else
    echo "Makefile.mak should be updated to Makefile.mak.new"
    echo "Differences:"
    echo "$diffs"
fi
