#!/bin/sh

NAME=qtbase-everywhere-opensource-src
VERSION=6.2.9
BUILD_ROOT=qtbase-everywhere-src-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://mirror.accum.se/mirror/qt.io/qtproject/archive/qt/6.2/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$BUILD_ROOT
    cd $DEV_PATH/$BUILD_ROOT
    cmd /c build.bat
    cmake --build . --parallel ${JOBS}
    cmake --install .
  ;;
  --clean)
    cd temp
    rm -rf $DEV_PATH/$BUILD_ROOT
    cd ..
  ;;
esac
