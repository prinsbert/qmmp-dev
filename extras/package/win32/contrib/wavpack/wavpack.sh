#!/bin/sh

NAME=wavpack
VERSION=5.7.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://github.com/dbry/WavPack/releases/download/$VERSION/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --host=${MINGW_HOST}
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
