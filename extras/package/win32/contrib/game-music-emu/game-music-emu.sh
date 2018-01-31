#!/bin/sh

NAME=game-music-emu
VERSION=0.6.2

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://bitbucket.org/mpyne/game-music-emu/downloads/$NAME-$VERSION.tar.xz -O $NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    cmake ./ -DCMAKE_INSTALL_PREFIX=$PREFIX -G "MSYS Makefiles" -DCMAKE_COLOR_MAKEFILE:BOOL=OFF
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
