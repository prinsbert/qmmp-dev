#!/bin/sh

NAME=qttools-everywhere-src
VERSION=6.1.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://ftp.acc.umu.se/mirror/qt.io/qtproject/official_releases/qt/6.1/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz
    cd $NAME-$VERSION
    CMAKE_PREFIX=${QT6_PATH} cmake ./ -DCMAKE_INSTALL_PREFIX=${QT6_PATH} -G "MSYS Makefiles"
    cmake --build . --parallel ${JOBS}
    cmake --install . 
  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
