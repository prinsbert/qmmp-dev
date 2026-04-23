#!/bin/sh

LIB_NAMES+='pkg-config yasm libmad mpg123 libogg libvorbis flac libsndfile opus opusfile '
LIB_NAMES+='libbs2b curl libcdio libcdio-paranoia libgnurx libcddb libgme libxmp musepack '
LIB_NAMES+='xa enca soxr librcd 7z '
LIB_NAMES+='libbinio adplug ' #adplug

if [ -n "`uname | grep 5.1`"  ]; then
    LIB_NAMES+='taglib-1.13 ffmpeg-3.4 libsidplayfp-2.3 wavpack-5.6 glew-2.2 projectm'
    export WGET_ARGS=--no-check-certificate
else
    LIB_NAMES+='utf8cpp taglib ffmpeg libsidplayfp wavpack glew projectm'
fi

export DEV_PATH=/c/devel

if [ "$1" == "--install-win64" ]; then
    export MINGW64_PATH=${DEV_PATH}/mingw64:${DEV_PATH}/mingw64/opt
    export ZLIB_ROOT=${DEV_PATH}/mingw64/x86_64-w64-mingw32
    export PREFIX=${DEV_PATH}/mingw64-libs
    export OPENSSL_ROOT_DIR=${DEV_PATH}/mingw64/opt
    export PATH=${PATH}:${DEV_PATH}/mingw64/bin:${DEV_PATH}/mingw64/opt/bin:${PREFIX}/bin:${DEV_PATH}/msys/bin
    export MINGW_HOST=x86_64-w64-mingw32
else
    export MINGW32_PATH=${DEV_PATH}/mingw32:${DEV_PATH}/mingw32/opt
    export ZLIB_ROOT=${DEV_PATH}/mingw32/i686-w64-mingw32
    export PREFIX=${DEV_PATH}/mingw32-libs
    export OPENSSL_ROOT_DIR=${DEV_PATH}/mingw32/opt
    export PATH=${PATH}:${DEV_PATH}/mingw32/bin:${DEV_PATH}/mingw32/opt/bin:${PREFIX}/bin:${DEV_PATH}/msys/bin
    export MINGW_HOST=i686-w64-mingw32
fi

export STRIP=false
export JOBS=4
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
export NO_COLOR=1 

case $1 in
  --download)
    
    if [ -n "$2" ]; then
        LIB_NAMES=$2
    fi
   
    for NAME in $LIB_NAMES
    do
        echo 'downloading '${NAME}'...'
        cd $NAME
        sh ./$NAME.sh $1
        cd ..
    done
  ;;
  --install|--install-win64)
    
    mkdir -p ${PREFIX} ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/share/doc

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
    echo "--download <name>"
    echo "--install <name>"
    echo "--install-win64 <name>"
    echo "--clean"
    echo "--clean-src"
    echo "--print-versions"
esac
