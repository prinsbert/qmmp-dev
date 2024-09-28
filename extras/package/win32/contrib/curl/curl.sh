#!/bin/sh

NAME=curl
VERSION=8.10.1

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://curl.haxx.se/download/$NAME-$VERSION.tar.gz
  ;;
  --install)
    cd temp
    tar xvzf $NAME-$VERSION.tar.gz
    cd $NAME-$VERSION
    cmake ./ -DCMAKE_INSTALL_PREFIX=${PREFIX} -G "MSYS Makefiles" -DZLIB_ROOT=${ZLIB_ROOT} \
    -DCMAKE_COLOR_MAKEFILE:BOOL=OFF -DBUILD_CURL_EXE=OFF -DCURL_USE_OPENSSL=ON \
    -DCURL_DISABLE_DICT=ON \
    -DCURL_DISABLE_FTP=ON \
    -DCURL_DISABLE_GOPHER=ON \
    -DCURL_DISABLE_IMAP=ON \
    -DCURL_DISABLE_GOPHER=ON \
    -DCURL_DISABLE_LDAP=ON \
    -DCURL_DISABLE_LDAPS=ON \
    -DCURL_DISABLE_MQTT=ON \
    -DCURL_DISABLE_POP3=ON \
    -DCURL_DISABLE_RTSP=ON \
    -DCURL_DISABLE_SMB=ON \
    -DCURL_DISABLE_TELNET=ON \
    -DCURL_DISABLE_TFTP=ON \
    -DCURL_DISABLE_SMTP=ON
    mingw32-make -j${JOBS}
    mingw32-make install

#CC=i686-w64-mingw32-gcc mingw32-make mingw32-ssl-zlib
    #mkdir -p ${PREFIX}/include/curl
    #cp -v include/curl/*.h ${PREFIX}/include/curl
    #cp -v lib/libcurl.dll ${PREFIX}/bin
    #cp -v lib/libcurl.dll.a ${PREFIX}/lib

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
