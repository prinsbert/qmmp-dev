#!/bin/sh

NAME=libsndfile
VERSION=1.0.28

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://www.mega-nerd.com/libsndfile/files/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cat ../../CVE-2017-8365.patch | patch -p1
    cat ../../CVE-2017-8363.patch | patch -p1
    cat ../../CVE-2017-8362.patch | patch -p1
    cat ../../CVE-2017-6892.patch | patch -p1
    cat ../../CVE-2019-3832.patch | patch -p1
    cat ../../binheader-heapoverflow.patch | patch -p1
    cat ../../fix_rf64_arm.patch | patch -p1
    cat ../../fix_typos.patch | patch -p1
    cat ../../a-ulaw-fix-multiple-buffer-overflows-432.patch | patch -p1
    cat ../../double64_init-Check-psf-sf.channels-against-upper-bo.patch | patch -p1
    cat ../../src-wav.c-Fix-heap-read-overflow.patch | patch -p1
    cat ../../Check-MAX_CHANNELS-in-sndfile-deinterleave.patch | patch -p1
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-external-libs --disable-sqlite
    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
