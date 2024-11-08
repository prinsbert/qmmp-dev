#!/bin/sh

LIB_NAMES+='qtbase qttools qttranslations qtwinextras qtimageformats'

export QT_VERSION=5.15.15
export DEV_PATH=/c/devel

if [ "$1" == "--install-win64" ]; then
    export MINGW64_PATH=${DEV_PATH}/mingw64
    export QT5_PATH=${DEV_PATH}/qt5-win64
    export ZLIB_ROOT=${MINGW32_PATH}/x86_64-w64-mingw32
    export PREFIX=${DEV_PATH}/qt5-win64
    export PATH=${PATH}:${MINGW64_PATH}/bin:${QT5_PATH}/bin:${PREFIX}/bin    
else
    export MINGW32_PATH=${DEV_PATH}/mingw32
    export QT5_PATH=${DEV_PATH}/qt5
    export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
    export PREFIX=${DEV_PATH}/qt5
    export PATH=${PATH}:${MINGW32_PATH}/bin:${QT5_PATH}/bin:${PREFIX}/bin
fi

export STRIP=false
export JOBS=2

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
  --install|--install-win64)
    mkdir -p ${PREFIX} ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/share/doc
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
        echo ${NAME}-${VERSION}
        cd ..
    done
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install"
    echo "--clean"
    echo "--clean-src"
    echo "--print-versions"
esac
