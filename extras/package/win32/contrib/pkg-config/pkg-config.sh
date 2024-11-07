#!/bin/sh

NAME=pkg-config
VERSION=0.23

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://download.gnome.org/binaries/win32/dependencies/${NAME}_${VERSION}-3_win32.zip
    wget -nc https://download.gnome.org/binaries/win64/dependencies/${NAME}_${VERSION}-2_win64.zip
  ;;
  --install)
    cd temp
    if [ ${MINGW64_PATH} ]; then
        unzip ${NAME}_${VERSION}-2_win64.zip -d ${PREFIX}/        
    else
        unzip ${NAME}_${VERSION}-3_win32.zip -d ${PREFIX}/ 
    fi
  ;;
  --clean)
  ;;
esac
