include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Engines/mpv

HEADERS += mpvenginefactory.h \
    mpvengine.h \
    mpvmetadatamodel.h

SOURCES += mpvengine.cpp \
    mpvenginefactory.cpp \
    mpvmetadatamodel.cpp

RESOURCES = translations/translations.qrc

PKGCONFIG += mpv libavcodec libavformat libavutil
target.path = $$PLUGIN_DIR/Engines
INSTALLS += target
