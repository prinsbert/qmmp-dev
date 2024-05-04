#!/bin/sh

LIB_NAMES+='pkg-config yasm taglib libmad mpg123 libogg libvorbis flac libsndfile opus opusfile '
LIB_NAMES+='libbs2b curl libcdio libcdio-paranoia libgnurx libcddb game-music-emu libxmp musepack '
LIB_NAMES+='glew projectm xa enca soxr librcd '
LIB_NAMES+='libbinio adplug ' #adplug

if [ -n "`uname | grep 5.1`"  ]; then
    LIB_NAMES+='ffmpeg-3.4 libsidplayfp-2.3 wavpack-5.6'
else
    LIB_NAMES+='ffmpeg libsidplayfp wavpack'
fi

export DEV_PATH=/c/devel
export MINGW32_PATH=${DEV_PATH}/mingw32:${DEV_PATH}/mingw32/opt
export QT4_PATH=${DEV_PATH}/qt4
export ZLIB_ROOT=${DEV_PATH}/mingw32/i686-w64-mingw32
export PREFIX=${DEV_PATH}/mingw32-libs
export OPENSSL_PATH=${DEV_PATH}/mingw32/opt

export PATH=${PATH}:${MINGW32_PATH}/bin:${QT4_PATH}/bin:${PREFIX}/bin:${DEV_PATH}/msys/bin

export LDFLAGS="-lssp"
export STRIP=false
export JOBS=2


mkdir -p ${PREFIX} ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/share/doc
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

case $1 in
  --download)
    for NAME in $LIB_NAMES
    do
        echo 'downloading '${NAME}'...'
        cd $NAME
        sh ./$NAME.sh $1
        cd ..
    done
  ;;
  --install)
    if [ -n "$2" ]; then
        LIB_NAMES=$2
    fi
     
    for NAME in $LIB_NAMES
    do
        echo 'installing '${NAME}'...'
        cd $NAME
        sh ./$NAME.sh --clean
        sh ./$NAME.sh --install
        sh ./$NAME.sh --clean
        cd ..
    done
    if [ "$STRIP" == "true" ]; then
        strip -v ${PREFIX}/bin/*.dll
    fi
    sh ./mingw-libs.sh --print-versions > ${PREFIX}/versions.txt
  ;;
  --clean)
    for NAME in $LIB_NAMES
    do
        echo 'cleaning '${NAME}'...'
        if [ -e $NAME/temp ]; then
          cd $NAME
          sh ./$NAME.sh --clean
          cd ..
        fi
    done

  ;;
  --clean-src)
    for NAME in $LIB_NAMES
    do
        echo 'cleaning '${NAME}'...'
        cd $NAME
        rm -rf temp
        cd ..
    done
  ;;
  --print-versions)
    for NAME in $LIB_NAMES
    do
        cd $NAME
        VERSION=`cat ./$NAME.sh | grep ^VERSION= | cut -d = -f 2`
        if [ $NAME = "ffmpeg-3.4" ]; then
           echo ffmpeg-${VERSION}
        else 
           echo ${NAME}-${VERSION}
        fi
        cd ..
    done
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install <name>"
    echo "--clean"
    echo "--clean-src"
    echo "--print-versions"
esac
