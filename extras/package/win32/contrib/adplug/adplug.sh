#!/bin/sh

NAME=adplug
VERSION=2.3.3

case $1 in
  --download)
    mkdir -p temp
    cd temp    
    wget -nc --no-check-certificate https://github.com/adplug/adplug/releases/download/$NAME-$VERSION/$NAME-$VERSION.tar.bz2 \
    -O $NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    cat ../../mingw32-build.patch | patch -p1
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
