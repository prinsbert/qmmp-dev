#Common settings for Qmmp Plugin Pack build

#Extra clean target

unix: QMAKE_DISTCLEAN += -r .build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui
MOC_DIR=./.build/moc
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

#Defines

DEFINES += QT_NO_CAST_FROM_BYTEARRAY QT_STRICT_ITERATORS QT_NO_FOREACH
unix:DEFINES += QMMP_WS_X11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050400 QT_DEPRECATED_WARNINGS

#Configuration

CONFIG += SVN_VERSION
CONFIG += hide_symbols
CONFIG += c++11
QT += widgets

#Version

QMMP_PLUGIN_PACK_VERSION = 1.4.0

#Install paths

VERSIONS = $$split(QMMP_PLUGIN_PACK_VERSION, ".")

QMMP_PLUGIN_PACK_VERSION_MAJOR = $$member(VERSIONS, 0)
QMMP_PLUGIN_PACK_VERSION_MINOR = $$member(VERSIONS, 1)

#APP_NAME_SUFFIX = "-1"

#QMAKE_LFLAGS_DEBUG += "-Wl,--as-needed -Wl,--no-undefined -Wl,-z,relro -Wl,--build-id -Wl,--enable-new-dtags"
#QMAKE_LFLAGS += "-Wl,--as-needed -Wl,--no-undefined -Wl,-z,relro -Wl,--build-id -Wl,--enable-new-dtags"

#*-g++ {
#  QMAKE_CXXFLAGS += -Werror=zero-as-null-pointer-constant
#  QMAKE_CXXFLAGS += -Werror=suggest-override
#}


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
  INCLUDEPATH += /usr/local/include
} else {
  INCLUDEPATH += C:/projects/qmmp-svn-trunk/qmmp/src
  QMAKE_LIBDIR += C:/projects/qmmp-svn-trunk/qmmp/bin
  EXTRA_INCDIR = C:/devel/mingw32-libs/include
  QMAKE_CXXFLAGS += "$${QMAKE_CFLAGS_ISYSTEM} $${EXTRA_INCDIR}"
  QMAKE_CFLAGS += "$${QMAKE_CFLAGS_ISYSTEM} $${EXTRA_INCDIR}"
  QMAKE_LIBDIR +=  C:/devel/mingw32-libs/lib
}

#Comment/uncomment this if you want to change plugins list

CONFIG += SRC_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += FFAP_YASM #assembler optimizations
CONFIG += XMP_PLUGIN
CONFIG += GOOM_PLUGIN
CONFIG += GOOM_ASM #assembler optimizations
CONFIG += FFVIDEO_PLUGIN
CONFIG += YOUTUBE_PLUGIN

CONFIG -= $$DISABLED_PLUGINS
