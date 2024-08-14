#!/bin/sh

NAME=utf8cpp
VERSION=4.0.5

case $1 in
  --download)
    mkdir -p temp
    cd temp    
    wget -nc https://github.com/nemtrif/utfcpp/archive/refs/tags/v${VERSION}.tar.gz 
  ;;

  --install)
    cd temp
    tar xvzf v${VERSION}.tar.gz
    cd utfcpp-$VERSION
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DBUILD_SHARED_LIBS=ON
    mingw32-make -j${JOBS}
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf utfcpp-$VERSION
    cd ..
  ;;
esac
