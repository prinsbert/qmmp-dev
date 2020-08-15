#!/bin/sh

NAME=libbinio
VERSION=1.5

case $1 in
  --download)
    mkdir -p temp
    cd temp    
    wget -nc --no-check-certificate https://github.com/adplug/libbinio/releases/download/$NAME-$VERSION/$NAME-$VERSION.tar.bz2 \
    -O $NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../mingw32-build.patch | patch -p1
    autoreconf -if
    ./configure --prefix=$PREFIX --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
