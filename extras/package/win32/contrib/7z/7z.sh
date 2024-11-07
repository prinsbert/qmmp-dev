#!/bin/sh

NAME=7z
VERSION=24.08
VERSION2=`echo ${VERSION} | cut -b 1-2,4-5`

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc https://sourceforge.net/projects/sevenzip/files/7-Zip/${VERSION}/7zr.exe
    wget -nc https://sourceforge.net/projects/sevenzip/files/7-Zip/${VERSION}/7z${VERSION2}-extra.7z
  ;;
  --install)
    cd temp
    ./7zr.exe x -y 7z${VERSION2}-extra.7z -o7z-extra
    if [ ${MINGW64_PATH} ]; then
        cp -vf 7z-extra/x64/7za.exe ${PREFIX}/bin        
    else
        cp -vf 7z-extra/7za.exe ${PREFIX}/bin 
    fi
  ;;
  --clean)
    cd temp
    rm -rf 7z-extra
    cd ..
  ;;
esac
