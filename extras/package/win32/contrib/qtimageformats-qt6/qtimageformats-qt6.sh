#!/bin/sh

NAME=qtimageformats-everywhere-opensource-src
VERSION=6.2.7
BUILD_ROOT=qtimageformats-everywhere-src-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://ftp.acc.umu.se/mirror/qt.io/qtproject/official_releases/qt/6.2/$VERSION/submodules/$NAME-$VERSION.tar.xz
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
