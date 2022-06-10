#!/bin/sh

QMMP_VERSION=2.1.1
QMMP_PLUGIN_PACK_VERSION=2.1.1

export DEV_PATH=/c/devel
export MINGW32_PATH=${DEV_PATH}/mingw32
export QT6_PATH=${DEV_PATH}/qt6
export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
export PREFIX=${DEV_PATH}/mingw32-libs
SVN_PATH=/c/Program\ Files\ \(x86\)/Subversion/bin
export PATH=${PATH}:${MINGW32_PATH}/bin:${QT6_PATH}/bin:${PREFIX}/bin:${SVN_PATH}
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

export JOBS=2



download_qmmp_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  wget -nc http://qmmp.ylsoftware.com/files/qmmp/2.1/qmmp-${QMMP_VERSION}.tar.bz2
  tar xvjf qmmp-${QMMP_VERSION}.tar.bz2
  cd ..
}

download_plugins_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  wget -nc http://qmmp.ylsoftware.com/files/qmmp-plugin-pack/2.1/qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
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
  cd ..
}

download_qmmp_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  #svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp qmmp-${QMMP_VERSION}
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-2.1 qmmp-${QMMP_VERSION}
  cd ..
}

download_plugins_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  #svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp-plugin-pack qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-plugin-pack-2.1 qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  cd ..
}

build ()
{ 
  cd qmmp-${QMMP_VERSION}
  qmake CONFIG+=release
  mingw32-make -j${JOBS}
  cd ..
  cd qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  qmake CONFIG+=release DISABLED_PLUGINS+=MODPLUG_PLUGIN INCLUDEPATH+=`dirs`/../qmmp-${QMMP_VERSION}/src QMAKE_LIBDIR+=`dirs`/../qmmp-${QMMP_VERSION}/bin
  mingw32-make -j${JOBS}
  cd ..
  cd qmmp-adplug-master
  qmake CONFIG+=release INCLUDEPATH+=`dirs`/../qmmp-${QMMP_VERSION}/src QMAKE_LIBDIR+=`dirs`/../qmmp-${QMMP_VERSION}/bin LIBS+=-lqmmp2 QT+=widgets CONFIG+=link_pkgconfig PKGCONFIG+=adplug \
  CONFIG+=hide_symbols
  mingw32-make -j${JOBS}
  cd ..
}

create_distr ()
{
  mkdir -p qmmp-distr
  cd qmmp-distr
  mkdir -p translations
  cp -v ../../*.txt ./
  cp -v ../../qmmp-2.x/*.txt ./
  cp -v ../../qmmp-2.x/*.nsi ./
  cp -v ../../qmmp-2.x/*.conf ./
  cp -v ../../qmmp-2.x/*.default ./
  cp -v ../../unzip.exe ./
  cp -rv ../../themes ./
  cp -rv ../../skins ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.exe ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.dll ./
  cp -rv ../qmmp-${QMMP_VERSION}/bin/plugins ./
  cp -rv ../qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}/bin/plugins ./
  find . -type f -name *.a -delete
  find . -type d -name ".svn" | xargs rm -rf
  cp -v ../qmmp-${QMMP_VERSION}/ChangeLog ./ChangeLog.txt
  cp -v ../qmmp-${QMMP_VERSION}/ChangeLog.rus ./ChangeLog.rus.txt
  u2d ./ChangeLog.txt
  u2d ./ChangeLog.rus.txt
  #versions
  #../../contrib/mingw-libs.sh --print-versions -v '' > versions.txt

  #Qt libs
  for LIB_NAME in Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Network.dll Qt6Sql.dll Qt6OpenGL.dll Qt6OpenGLWidgets.dll
  do
    cp -v ${QT6_PATH}/bin/${LIB_NAME} ./
  done
  #Qt plugins
  mkdir -p plugins/imageformats plugins/platforms plugins/sqldrivers plugins/styles
  cp -v ${QT6_PATH}/plugins/imageformats/*.dll ./plugins/imageformats  
  for LIB_NAME in qwindows.dll
  do
    cp -v ${QT6_PATH}/plugins/platforms/${LIB_NAME} ./plugins/platforms
  done
  for LIB_NAME in qsqlite.dll
  do
    cp -v ${QT6_PATH}/plugins/sqldrivers/${LIB_NAME} ./plugins/sqldrivers
  done
  for LIB_NAME in qwindowsvistastyle.dll
  do
    cp -v ${QT6_PATH}/plugins/styles/${LIB_NAME} ./plugins/styles
  done
  #translations
  cp -v ${QT6_PATH}/translations/qtbase_??.qm ./translations
  #mingw32 libs
  for LIB_NAME in libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll libgomp-1.dll libssp-0.dll
  do
    cp -v ${MINGW32_PATH}/bin/${LIB_NAME} ./
  done
  #third party libs   
  for LIB_NAME in avcodec-*.dll avformat-*.dll avutil-*.dll glew32.dll libFLAC-8.dll libcddb-2.dll libcdio-19.dll libcdio_cdda-2.dll libcdio_paranoia-2.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libcurl.dll libenca-0.dll libgme.dll libgnurx-0.dll libmad-0.dll libxmp.dll libmpcdec.dll libogg-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libopus-0.dll libopusfile-0.dll libprojectM.dll libsidplayfp-*.dll libsndfile-1.dll libtag.dll libvorbis-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libvorbisfile-3.dll libwavpack-1.dll libsoxr.dll libmpg123-0.dll librcd-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  #projectM presets
  cp -rv ${PREFIX}/share/projectM/ ./	     
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
    download_qmmp_tarball
    #download_plugins_tarball
    #download_qmmp_svn
    download_plugins_svn
    download_qmmp_adplug_archive
  ;;
  --install)
    cd tmp
    build
    create_distr
    find qmmp-distr -type f -name *.dll   | xargs strip -v
    find qmmp-distr -type f -name *.exe   | xargs strip -v 
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
    echo "--install"
    echo "--clean"
    echo "--clean-src"
  ;;
esac
