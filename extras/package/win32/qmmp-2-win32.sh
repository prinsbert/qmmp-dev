#!/bin/sh

QMMP_VERSION=2.3.0
QMMP_PLUGIN_PACK_VERSION=2.3.0

export DEV_PATH=/c/devel

if [ "$1" == "--install-win64" ]; then
  export MINGW32_PATH=${DEV_PATH}/mingw64
  export QT6_PATH=${DEV_PATH}/qt6-win64
  export ZLIB_ROOT=${MINGW32_PATH}/mingw64/x86_64-w64-mingw32
  export PREFIX=${DEV_PATH}/mingw64-libs
else
  export MINGW32_PATH=${DEV_PATH}/mingw32
  export QT6_PATH=${DEV_PATH}/qt6
  export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
  export PREFIX=${DEV_PATH}/mingw32-libs
fi

SVN_PATH=/c/Program\ Files/SlikSvn/bin
export PATH=${PATH}:${MINGW32_PATH}/bin:${QT6_PATH}/bin:${PREFIX}/bin:${SVN_PATH}
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:${QT6_PATH}/lib/pkgconfig 

export JOBS=4

download_qmmp_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  wget -nc http://qmmp.ylsoftware.com/files/qmmp/2.2/qmmp-${QMMP_VERSION}.tar.bz2
  tar xvjf qmmp-${QMMP_VERSION}.tar.bz2
  cd ..
}

download_plugins_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  wget -nc http://qmmp.ylsoftware.com/files/qmmp-plugin-pack/2.2/qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  tar xvjf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  cd ..
}

download_qmmp_adplug_archive()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-adplug...'
  wget -nc https://github.com/cspiegel/qmmp-adplug/archive/master.zip
  7za x -y master.zip
  cat ../adplug.patch | patch -p2 -d qmmp-adplug-master
  cd ..
}

download_qmmp_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp qmmp-${QMMP_VERSION}
  #svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-2.2 qmmp-${QMMP_VERSION}
  cd ..
}

download_plugins_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp-plugin-pack qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  #svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-plugin-pack-2.2 qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  cd ..
}

build ()
{
  QMMP_INSTALL_PREFIX=`dirs`/qmmp-distr 
  cd qmmp-${QMMP_VERSION}
  cmake . -G "MSYS Makefiles" -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_LIBRCD=ON -DUSE_FILEWRITER=OFF -DUSE_DIR_ASSOC=OFF \
  -DCMAKE_REQUIRED_INCLUDES=${PREFIX}/include \
  -DCMAKE_SYSTEM_LIBRARY_PATH=${PREFIX}/lib \
  -DCMAKE_INSTALL_PREFIX=${QMMP_INSTALL_PREFIX}
  cmake --build . -j${JOBS}
  cmake --install .
  cd ..
  cd qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${QMMP_INSTALL_PREFIX}/lib/pkgconfig  cmake . -G "MSYS Makefiles" -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_YTB=OFF -DUSE_MMS=OFF -DUSE_FFAP=OFF -DUSE_FFVIDEO=OFF -DUSE_MPLAYER=OFF \
  -DCMAKE_REQUIRED_INCLUDES=${PREFIX}/include \
  -DCMAKE_SYSTEM_LIBRARY_PATH=${PREFIX}/lib \
  -DCMAKE_INSTALL_PREFIX=${QMMP_INSTALL_PREFIX}
  cmake --build . -j${JOBS}
  cmake --install .
  cd ..
  cd qmmp-adplug-master
  qmake CONFIG+=release INCLUDEPATH+=${QMMP_INSTALL_PREFIX}/include QMAKE_LIBDIR+=${QMMP_INSTALL_PREFIX}/lib LIBS+=-lqmmp2 QT+=widgets CONFIG+=link_pkgconfig PKGCONFIG+=adplug \
  CONFIG+=hide_symbols
  mingw32-make -j${JOBS}
  cd ..
}

create_distr ()
{
  mkdir -p qmmp-distr
  cd qmmp-distr
  mkdir -p nsis-translations ./share/qmmp
  cp -v ../../*.txt ./
  cp -v ../../qmmp-2.x/*.txt ./
  cp -v ../../qmmp-2.x/*.nsi ./
  cp -v ../../qmmp-2.x/nsis-translations/*.nsh ./nsis-translations
  cp -v ../../qmmp-2.x/*.conf ./bin
  cp -v ${PREFIX}/bin/7za.exe ./bin 
  cp -rv ../../themes ./share/    
  cp -rv ../../skins ./share/qmmp/
  #cp -v ../qmmp-${QMMP_VERSION}/bin/*.exe ./
  #cp -v ../qmmp-${QMMP_VERSION}/bin/*.dll ./
  #cp -rv ../qmmp-${QMMP_VERSION}/bin/plugins ./
  #cp -rv ../qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}/bin/plugins ./
  #find . -type f -name *.a -delete
  #find . -type d -name ".svn" | xargs rm -rf
  cp -v ../qmmp-${QMMP_VERSION}/ChangeLog ./ChangeLog.txt
  cp -v ../qmmp-${QMMP_VERSION}/ChangeLog.rus ./ChangeLog.rus.txt
  u2d ./ChangeLog.txt
  u2d ./ChangeLog.rus.txt
  #versions
  #../../contrib/mingw-libs.sh --print-versions -v '' > versions.txt

  #Qt libs
  for LIB_NAME in Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll Qt6Sql.dll Qt6OpenGL.dll Qt6OpenGLWidgets.dll
  do
    cp -v ${QT6_PATH}/bin/${LIB_NAME} ./bin
  done
  #Qt plugins
  mkdir -p lib/qt6/plugins/imageformats lib/qt6/plugins/platforms lib/qt6/plugins/sqldrivers lib/qt6/plugins/styles lib/qt6/plugins/tls
  cp -v ${QT6_PATH}/plugins/imageformats/*.dll ./lib/qt6/plugins/imageformats  
  for LIB_NAME in qwindows.dll
  do
    cp -v ${QT6_PATH}/plugins/platforms/${LIB_NAME} ./lib/qt6/plugins/platforms
  done
  for LIB_NAME in qsqlite.dll
  do
    cp -v ${QT6_PATH}/plugins/sqldrivers/${LIB_NAME} ./lib/qt6/plugins/sqldrivers
  done
  for LIB_NAME in qmodernwindowsstyle.dll
  do
    cp -v ${QT6_PATH}/plugins/styles/${LIB_NAME} ./lib/qt6/plugins/styles
  done
  cp -v ${QT6_PATH}/plugins/tls/*.dll ./lib/qt6/plugins/tls
  #translations
  mkdir -p ./lib/qt6/translations
  cp -v ${QT6_PATH}/translations/qtbase_??.qm ./lib/qt6/translations
  #mingw32 libs
  for LIB_NAME in libgcc_s_dw2-1.dll libgcc_s_sjlj-1.dll libstdc++-6.dll libwinpthread-1.dll libgomp-1.dll libssp-0.dll
  do
    cp -v ${MINGW32_PATH}/bin/${LIB_NAME} ./bin
  done
  for LIB_NAME in libcrypto-1_1.dll libssl-1_1.dll libcrypto-1_1-x64.dll libssl-1_1-x64.dll
  do
    cp -v ${MINGW32_PATH}/opt/bin/${LIB_NAME} ./bin
  done
  #third party libs   
  for LIB_NAME in avcodec-*.dll avformat-*.dll avutil-*.dll glew32.dll libFLAC-*.dll libcddb-2.dll libcdio-19.dll libcdio_cdda-2.dll libcdio_paranoia-2.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./bin
  done
  for LIB_NAME in libcurl.dll libenca-0.dll libgme.dll libgnurx-0.dll libmad-0.dll libxmp.dll libmpcdec.dll libogg-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./bin
  done
  for LIB_NAME in libopus-0.dll libopusfile-0.dll libprojectM.dll libsidplayfp-*.dll libsndfile-1.dll libtag.dll libvorbis-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./bin
  done
  for LIB_NAME in libvorbisfile-3.dll libwavpack-1.dll libsoxr.dll libmpg123-0.dll librcd-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./bin
  done
  #projectM presets
  cp -rv ${PREFIX}/share/projectM/ ./share	     
  #adplug
  mkdir -p adplug
  for LIB_NAME in libadplug*.dll libbinio*.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./adplug/
  done
  cp -v ../qmmp-adplug-master/release/*.dll ./adplug/
  cd ..
}


case $1 in
  --download)
    #download_qmmp_tarball
    #download_plugins_tarball
    download_qmmp_svn
    download_plugins_svn
    download_qmmp_adplug_archive
  ;;
  --download-svn)
    download_qmmp_svn
    download_plugins_svn
    download_qmmp_adplug_archive
  ;;
  --install|--install-win64)
    cd tmp
    build
    create_distr
    find qmmp-distr -type f -name *.dll   | xargs strip -v
    find qmmp-distr -type f -name *.exe -not -name 7za.exe  | xargs strip -v
    if [ "$1" == "--install" ]; then
      sed '/!define WIN64/d' -i ./qmmp-distr/qmmp.nsi
    fi 
  ;;
  --clean)
    cd tmp
    rm -rf qmmp-distr
    rm -rf qmmp-${QMMP_VERSION}
    rm -rf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  ;;
  --clean-src)
    rm -rf tmp
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--download-svn"
    echo "--install"
    echo "--install-win64"
    echo "--clean"
    echo "--clean-src"
  ;;
esac
