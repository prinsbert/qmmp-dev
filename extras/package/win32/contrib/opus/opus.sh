#!/bin/sh

NAME=opus
VERSION=1.6.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://ftp.osuosl.org/pub/xiph/releases/opus/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-extra-programs
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
