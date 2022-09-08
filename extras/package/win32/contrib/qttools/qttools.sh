#!/bin/sh

NAME=qttools-everywhere-opensource-src
VERSION=5.15.6
BUILD_ROOT=qttools-everywhere-src-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://ftp.acc.umu.se/mirror/qt.io/qtproject/official_releases/qt/5.15/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $BUILD_ROOT
    qmake
    mingw32-make -j${JOBS}
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf $BUILD_ROOT
    cd ..
  ;;
esac
