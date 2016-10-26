#!/bin/sh

NAME=taglib
VERSION=1.11.1

case $1 in
  --download)
    mkdir -p temp
    cd temp    
    wget -nc --no-check-certificate https://github.com/taglib/taglib/releases/download/v$VERSION/$NAME-$VERSION.tar.gz \
    -O $NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" -DZLIB_ROOT=${ZLIB_ROOT} \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DBUILD_SHARED_LIBS=ON
    mingw32-make -j${JOBS}
    mingw32-make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
