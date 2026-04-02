#!/bin/sh

LIB_NAMES+='qtbase-qt6 qttools-qt6 qttranslations-qt6 qtimageformats-qt6'

export QT_VERSION=6.10.3
export DEV_PATH=/c/devel


if [ "$1" == "--install-win64" ]; then
    export PREFIX=C:\\devel\\qt6-win64
    export MINGW64_PATH=${DEV_PATH}/mingw64
    export QT6_PATH=${DEV_PATH}/qt6-win64
    export ZLIB_ROOT=${MINGW32_PATH}/x86_64-w64-mingw32
    export PATH=${PATH}:${MINGW64_PATH}/bin:${QT6_PATH}/bin:${PREFIX}/bin
else
    export PREFIX=C:\\devel\\qt6
    export MINGW32_PATH=${DEV_PATH}/mingw32
    export QT6_PATH=${DEV_PATH}/qt6
    export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
    export PATH=${PATH}:${MINGW32_PATH}/bin:${QT6_PATH}/bin:${PREFIX}/bin
fi

export STRIP=false
export JOBS=4

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
        echo ${NAME}-${QT_VERSION}
    done
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install"
    echo "--install-win64"
    echo "--clean"
    echo "--clean-src"
    echo "--print-versions"
esac
