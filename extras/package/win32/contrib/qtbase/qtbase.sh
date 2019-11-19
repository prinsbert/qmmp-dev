#!/bin/sh

NAME=qtbase-everywhere-src
VERSION=5.12.6

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc --no-check-certificate https://download.qt.io/official_releases/qt/5.12/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$NAME-$VERSION
    cd $DEV_PATH/$NAME-$VERSION
    cmd /c build.bat
    mingw32-make -j${JOBS}
    mingw32-make install
  ;;
  --clean)
    cd temp
    rm -rf $DEV_PATH/$NAME-$VERSION
    cd ..
  ;;
esac
