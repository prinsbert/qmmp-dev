#!/bin/sh

NAME=qtimageformats-everywhere-src
VERSION=6.7.3
BUILD_ROOT=$NAME-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://mirror.accum.se/mirror/qt.io/qtproject/archive/qt/6.7/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $BUILD_ROOT
    CMAKE_PREFIX=${QT6_PATH} cmake ./ -DCMAKE_INSTALL_PREFIX=${QT6_PATH} -GNinja
    cmake --build . --parallel ${JOBS}
    cmake --install . 
  ;;
  --clean)
    cd temp
    rm -rf $BUILD_ROOT
    cd ..
  ;;
esac
