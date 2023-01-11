#!/bin/sh

NAME=libsndfile
VERSION=1.2.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://github.com/libsndfile/libsndfile/releases/download/$VERSION/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-external-libs --disable-sqlite
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
