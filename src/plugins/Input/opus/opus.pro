include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/opus

HEADERS += decoderopusfactory.h \
    decoder_opus.h \
    opusmetadatamodel.h \
    opusproperties.h \
    opusfile.h \
    tdebug.h


SOURCES += decoder_opus.cpp \
    decoderopusfactory.cpp \
    opusmetadatamodel.cpp \
    opusproperties.cpp \
    opusfile.cpp \
    tdebug.cpp


RESOURCES = translations/translations.qrc

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += taglib opus opusfile
}

win32 {
    LIBS += -lopusfile -lopus -ltag.dll -lm
}
