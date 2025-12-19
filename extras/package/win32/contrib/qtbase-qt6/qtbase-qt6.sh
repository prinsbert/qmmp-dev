#!/bin/sh

NAME=qtbase-everywhere-src
VERSION=${QT_VERSION}
BUILD_ROOT=$NAME-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://mirror.accum.se/mirror/qt.io/qtproject/archive/qt/6.10/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$BUILD_ROOT
    cp ../build-win64.bat $DEV_PATH/$BUILD_ROOT
    cd $DEV_PATH/$BUILD_ROOT
    if [ ${MINGW64_PATH} ]; then
        cmd /c build-win64.bat
    else
        cmd /c build.bat
    fi
    cmake --build . --parallel ${JOBS}
    cmake --install .
  ;;
  --clean)
    cd temp
    rm -rf $DEV_PATH/$BUILD_ROOT
    cd ..
  ;;
esac
