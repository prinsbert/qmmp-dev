#!/bin/sh

NAME=libxmp
VERSION=4.6.0

case $1 in
  --download)
    mkdir -p temp
    cd temp    
    wget -nc --no-check-certificate https://github.com/libxmp/libxmp/releases/download/${NAME}-${VERSION}/${NAME}-${VERSION}.tar.gz \
    -O $NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
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
