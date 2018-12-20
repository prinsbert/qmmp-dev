#!/bin/sh

NAME=qtwinextras-everywhere-src
VERSION=5.11.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://download.qt.io/official_releases/qt/5.11/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    qmake
    mingw32-make -j${JOBS}
    mingw32-make install
  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
