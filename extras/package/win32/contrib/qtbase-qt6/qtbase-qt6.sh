#!/bin/sh

NAME=qtbase-everywhere-src
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
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$BUILD_ROOT
    cat ../QTBUG-129434.patch | patch -p1 -d $DEV_PATH/$BUILD_ROOT
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
