#!/bin/sh

NAME=xa
VERSION=2.3.14

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ftp.jaist.ac.jp/pub/Linux/Gentoo/distfiles/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    make mingw -j${JOBS}
    cp *.exe ${PREFIX}/bin

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
