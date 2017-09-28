include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Engines/ffvideo

HEADERS += ffvideofactory.h \
    ffmpegengine.h \
    ffvideometadatamodel.h \
    packetbuffer.h \
    audiothread.h \
    videothread.h \
    videowindow.h \
    ffvideodecoder.h

SOURCES += ffmpegengine.cpp \
    ffvideofactory.cpp \
    ffvideometadatamodel.cpp \
    packetbuffer.cpp \
    audiothread.cpp \
    videothread.cpp \
    videowindow.cpp \
    ffvideodecoder.cpp

#FORMS += settingsdialog.ui

#RESOURCES = translations/translations.qrc

DEFINES += __STDC_CONSTANT_MACROS

target.path = $$LIB_DIR/qmmp/Engines
INSTALLS += target
PKGCONFIG += libavcodec libavformat libavutil libswscale
