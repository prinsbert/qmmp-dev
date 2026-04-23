#!/bin/sh

NAME=libgme
VERSION=0.6.5

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc ${WGET_ARGS} https://github.com/libgme/game-music-emu/releases/download/$VERSION/$NAME-$VERSION-src.tar.gz -O $NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cmake ./ -DCMAKE_INSTALL_PREFIX=$PREFIX -G "MSYS Makefiles" -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DENABLE_UBSAN:BOOL=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
