#Common settings for Qmmp Plugin Pack build

#Extra clean target

unix: QMAKE_DISTCLEAN += -r .build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui
MOC_DIR=./.build/moc
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

#Defines

DEFINES += QT_NO_CAST_FROM_BYTEARRAY QT_STRICT_ITERATORS QT_NO_FOREACH QT_MESSAGELOGCONTEXT
unix:DEFINES += QMMP_WS_X11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060400 QT_DEPRECATED_WARNINGS QT_NO_CAST_FROM_ASCII

#Configuration

CONFIG += SVN_VERSION
CONFIG += hide_symbols
CONFIG += c++17
QT += widgets

#Version

QMMP_PLUGIN_PACK_VERSION = 2.3.0

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
  
  win32 {
    contains(QT_ARCH, x86_64) {
      EXTRA_INCDIR = C:/devel/mingw64-libs/include
      QMAKE_LIBDIR +=  C:/devel/mingw64-libs/lib 
    } else {
      EXTRA_INCDIR = C:/devel/mingw32-libs/include 
      QMAKE_LIBDIR +=  C:/devel/mingw32-libs/lib
    }
    QMAKE_CXXFLAGS += "-isystem $${EXTRA_INCDIR}"
    QMAKE_CFLAGS += "-isystem $${EXTRA_INCDIR}"
  }
}

#Comment/uncomment this if you want to change plugins list

CONFIG += SRC_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += MODPLUG_PLUGIN
CONFIG += GOOM_PLUGIN
CONFIG += GOOM_ASM #assembler optimizations
CONFIG += FFVIDEO_PLUGIN
CONFIG += YOUTUBE_PLUGIN

CONFIG -= $$DISABLED_PLUGINS
