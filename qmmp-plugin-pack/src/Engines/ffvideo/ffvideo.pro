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

RESOURCES = translations/translations.qrc

DEFINES += __STDC_CONSTANT_MACROS

PKGCONFIG += libavcodec libavformat libavutil libswscale libswresample

QMAKE_CXXFLAGS += -Wno-missing-field-initializers

target.path = $$PLUGIN_DIR/Engines
INSTALLS += target
