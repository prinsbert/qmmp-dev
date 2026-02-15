#!/bin/sh

NAME=libsidplayfp
VERSION=2.16.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://sourceforge.net/projects/sidplay-residfp/files/$NAME/2.16/$NAME-$VERSION.tar.gz
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
