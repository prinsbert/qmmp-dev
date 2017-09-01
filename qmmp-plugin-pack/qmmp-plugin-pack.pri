#Common settings for Qmmp Plugin Pack build

#Extra clean target

unix: QMAKE_DISTCLEAN += -r .build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui
MOC_DIR=./.build/moc
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

unix{
INCLUDEPATH += /home/user/qmmp-0.11/include
QMAKE_LIBDIR += /home/user/qmmp-0.11/lib
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include
}else{
INCLUDEPATH += C:/projects/qmmp-svn-trunk/qmmp/src
QMAKE_LIBDIR += C:/projects/qmmp-svn-trunk/qmmp/bin
}

#Version

QMMP_PLUGIN_PACK_VERSION = 0.11.0
CONFIG += SVN_VERSION


#Comment/uncomment this if you want to change plugins list

CONFIG += SRC_PLUGIN
CONFIG += MPG123_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += FFAP_YASM #assembler optimizations
CONFIG += XMP_PLUGIN
CONFIG += GOOM_PLUGIN
CONFIG += GOOM_ASM #assembler optimizations
CONFIG += HISTORY_PLUGIN

CONFIG -= $$DISABLED_PLUGINS
