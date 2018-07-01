include(../qmmp-plugin-pack.pri)
unix:PLUGINS_PREFIX=../../../lib/qmmp
win32:PLUGINS_PREFIX=../../../../bin/plugins

win32 {
    QMAKE_LIBDIR += ../../../../bin
    PLUGINS_PREFIX=../../../../bin/plugins
    LIBS += -lqmmp0
}

unix {
    PLUGINS_PREFIX=../../../lib/qmmp-$${QMMP_PLUGIN_PACK_VERSION_MAJOR}.$${QMMP_PLUGIN_PACK_VERSION_MINOR}
    PKGCONFIG += qmmp$${APP_NAME_SUFFIX}
}

CONFIG += warn_on plugin lib link_pkgconfig hide_symbols

TEMPLATE = lib
