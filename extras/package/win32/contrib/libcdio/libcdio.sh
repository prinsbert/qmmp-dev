#!/bin/sh

NAME=libcdio
VERSION=2.3.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://github.com/libcdio/libcdio/releases/download/$VERSION/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    autoreconf -fi
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-cxx --disable-example-progs \
    --disable-cddb --without-cd-drive --without-cd-info --without-cdda-player \
    --without-iso-info --without-iso-read --without-cd-read --build=${MINGW_HOST} --host=${MINGW_HOST}
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
