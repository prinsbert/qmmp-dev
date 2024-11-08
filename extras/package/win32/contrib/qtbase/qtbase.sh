#!/bin/sh

NAME=qtbase-everywhere-opensource-src
VERSION=${QT_VERSION}
BUILD_ROOT=qtbase-everywhere-src-$VERSION

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://ftp.acc.umu.se/mirror/qt.io/qtproject/official_releases/qt/5.15/$VERSION/submodules/$NAME-$VERSION.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION.tar.xz -C $DEV_PATH
    cp ../build.bat $DEV_PATH/$BUILD_ROOT
    cp ../build-win64.bat $DEV_PATH/$BUILD_ROOT
    cat ../mingw_w64_9.0.patch | patch -p1 -d $DEV_PATH/$BUILD_ROOT
    cd $DEV_PATH/$BUILD_ROOT
    if [ ${MINGW64_PATH} ]; then
        cmd /c build-win64.bat
    else
        cmd /c build.bat
    fi
    mingw32-make -j${JOBS}
    mingw32-make install
  ;;
  --clean)
    cd temp
    rm -rf $DEV_PATH/$BUILD_ROOT
    cd ..
  ;;
esac
