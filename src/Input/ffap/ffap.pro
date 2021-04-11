include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/ffap

HEADERS += decoderffapfactory.h ffap.h decoder_ffap.h \
    ffapmetadatamodel.h \
    decoder_ffapcue.h

SOURCES += decoderffapfactory.cpp ffap.c decoder_ffap.cpp \
    ffapmetadatamodel.cpp \
    decoder_ffapcue.cpp

RESOURCES = translations/translations.qrc

unix {
    PKGCONFIG += taglib
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
}
win32 {
    LIBS += -ltag.dll
}
