#!/bin/sh

NAME=glew
VERSION=2.2.0

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://downloads.sourceforge.net/sourceforge/glew/glew/${VERSION}/${NAME}-${VERSION}.tgz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tgz
    cd $NAME-$VERSION
    cd build
    cmake ./cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DBUILD_UTILS=OFF -G "MSYS Makefiles" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 
    make
    make install
    cd ..
  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
