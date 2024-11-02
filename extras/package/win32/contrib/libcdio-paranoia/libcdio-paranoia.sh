#!/bin/sh

NAME=libcdio-paranoia
VERSION=10.2+2.0.2

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
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cxx --disable-example-progs --build=${MINGW_HOST} --host=${MINGW_HOST}
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
