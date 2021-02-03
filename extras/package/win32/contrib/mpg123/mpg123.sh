#!/bin/sh

NAME=mpg123
VERSION=1.26.4

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://www.mpg123.de/download/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    
    autoreconf -i
    ./configure --prefix=$PREFIX --disable-debugging --enable-shared --disable-static
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
