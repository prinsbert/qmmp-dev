#Common settings for Qmmp Plugin Pack build

#Extra clean target

unix: QMAKE_DISTCLEAN += -r .build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui
MOC_DIR=./.build/moc
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

QT += widgets

#Version

QMMP_PLUGIN_PACK_VERSION = 1.3.0
CONFIG += SVN_VERSION

#Install paths

VERSIONS = $$split(QMMP_PLUGIN_PACK_VERSION, ".")

QMMP_PLUGIN_PACK_VERSION_MAJOR = $$member(VERSIONS, 0)
QMMP_PLUGIN_PACK_VERSION_MINOR = $$member(VERSIONS, 1)

#APP_NAME_SUFFIX = "-1"

unix {
  isEmpty(PREFIX): PREFIX=/usr
  isEmpty(BIN_DIR): BIN_DIR=$$PREFIX/bin
  isEmpty(DATA_DIR): DATA_DIR=$$PREFIX/share
  isEmpty(LIB_DIR): LIB_DIR=$$PREFIX/lib
  isEmpty(PLUGIN_DIR): PLUGIN_DIR=$$LIB_DIR/qmmp-$${QMMP_PLUGIN_PACK_VERSION_MAJOR}.$${QMMP_PLUGIN_PACK_VERSION_MINOR}
}

#Includes

unix {
  INCLUDEPATH += /home/user/qmmp-$${QMMP_PLUGIN_PACK_VERSION_MAJOR}.$${QMMP_PLUGIN_PACK_VERSION_MINOR}/include
  QMAKE_LIBDIR += /home/user/qmmp-$${QMMP_PLUGIN_PACK_VERSION_MAJOR}.$${QMMP_PLUGIN_PACK_VERSION_MINOR}/lib
  INCLUDEPATH += /usr/include
  INCLUDEPATH += /usr/local/include
} else {
  INCLUDEPATH += C:/projects/qmmp-svn-trunk/qmmp/src
  QMAKE_LIBDIR += C:/projects/qmmp-svn-trunk/qmmp/bin
}

#Comment/uncomment this if you want to change plugins list

CONFIG += SRC_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += FFAP_YASM #assembler optimizations
CONFIG += XMP_PLUGIN
CONFIG += GOOM_PLUGIN
CONFIG += GOOM_ASM #assembler optimizations
CONFIG += HISTORY_PLUGIN
CONFIG += FFVIDEO_PLUGIN

CONFIG -= $$DISABLED_PLUGINS
