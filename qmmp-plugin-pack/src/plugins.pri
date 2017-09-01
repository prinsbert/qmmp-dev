include(../qmmp-plugin-pack.pri)

win32 {
    QMAKE_LIBDIR += ../../../../bin
    PLUGINS_PREFIX=../../../../bin/plugins
    LIBS += -lqmmp0
}

unix {
    isEmpty(LIB_DIR){
        LIB_DIR = /lib
    }
    PLUGINS_PREFIX=../../../lib/qmmp
    PKGCONFIG += qmmp
}

CONFIG += warn_on plugin lib link_pkgconfig
TEMPLATE = lib
