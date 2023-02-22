#!/bin/sh

NAME=libxml2
VERSION=2.10.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget --no-check-certificate -nc https://download.gnome.org/sources/libxml2/2.10/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --without-http --without-debug --without-docbook \
    --without-html --without-modules --without-python --with-zlib=no  --without-html --without-ftp
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
