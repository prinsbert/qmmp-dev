#!/bin/sh

NAME=soxr
VERSION=0.1.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://sourceforge.net/projects/soxr/files/$NAME-$VERSION-Source.tar.xz
  ;;
  --install)
    cd temp
    tar xvJf $NAME-$VERSION-Source.tar.xz
    cd $NAME-$VERSION-Source
    cat ../../soxr-pkg-config.patch | patch -p1
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE=Release
    mingw32-make -j${JOBS}
    mingw32-make install
  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION-Source
    cd ..
  ;;
esac
