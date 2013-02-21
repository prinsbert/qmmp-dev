#Common settings for Qmmp Plugin Pack build

#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj

unix{
INCLUDEPATH += /home/user/qmmp-0.6/include
INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include
}else{
INCLUDEPATH += C:/projects/qmmp/qmmp-svn-stable/qmmp-0.6/src
QMAKE_LIBDIR += C:/projects/qmmp/qmmp-svn-stable/qmmp-0.6/bin
INCLUDEPATH += D:/projects/qmmp/qmmp-svn-stable/qmmp-0.6/src
QMAKE_LIBDIR += D:/projects/qmmp/qmmp-svn-stable/qmmp-0.6/bin
}

#Version

QMMP_PLUGIN_PACK_VERSION = 0.6.6
CONFIG += SVN_VERSION


#Comment/uncomment this if you want to change plugins list

CONFIG += MPG123_PLUGIN
CONFIG += FFAP_PLUGIN
CONFIG += SIMPLE_UI_PLUGIN
