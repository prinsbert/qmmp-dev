#!/bin/sh

NAME=librcd
VERSION=0.1.14

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://github.com/RusXMMS/librcd/archive/refs/heads/master.zip
  ;;
  --install)
    cd temp
    unzip master.zip
    cd $NAME-master
    cat ../../librcd-mingw32.patch | patch -p1
    ./autogen.sh
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
