#!/bin/sh

NAME=qtbase-everywhere-src
VERSION=6.1.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://download.qt.io/official_releases/qt/6.1/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$NAME-$VERSION
    cd $DEV_PATH/$NAME-$VERSION
    cmd /c build.bat
    cmake --build . --parallel ${JOBS}
    cmake --install .
  ;;
  --clean)
    cd temp
    rm -rf $DEV_PATH/$NAME-$VERSION
    cd ..
  ;;
esac
