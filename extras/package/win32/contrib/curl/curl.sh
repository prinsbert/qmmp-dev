#!/bin/sh

NAME=curl
VERSION=8.3.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://curl.haxx.se/download/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    CC=i686-w64-mingw32-gcc mingw32-make mingw32-ssl-zlib
    mkdir -p ${PREFIX}/include/curl
    cp -v include/curl/*.h ${PREFIX}/include/curl
    cp -v lib/libcurl.dll ${PREFIX}/bin
    cp -v lib/libcurl.dll.a ${PREFIX}/lib

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
