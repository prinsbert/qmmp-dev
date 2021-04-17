#!/bin/sh

NAME=libcdio-paranoia
VERSION=10.2+2.0.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.gnu.org/gnu/libcdio/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    autoreconf -fi
    CFLAGS="-march=i686" ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cxx --disable-example-progs
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
