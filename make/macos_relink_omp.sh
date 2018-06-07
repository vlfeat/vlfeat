#!/bin/bash
echo "Relinking OMP for binary $1"
binary=$1
libpath=$(otool -l "$binary"| grep libomp.dylib  | sed -En 's/ *name ([\/.[:alnum:]]*).*/\1/p')
install_name_tool -change "$libpath" "@loader_path/libomp.dylib" "$binary"
rsync -v "$libpath" $(dirname "$binary")/libomp.dylib
