#!/bin/sh

NAME=libsndfile
VERSION=1.0.31

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://github.com/libsndfile/libsndfile/releases/download/$VERSION/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
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
