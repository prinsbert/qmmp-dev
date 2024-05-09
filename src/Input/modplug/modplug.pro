include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/modplug

HEADERS += decodermodplugfactory.h \
    decoder_modplug.h \
    archivereader.h \
    modplugmetadatamodel.h \
    modplugsettingsdialog.h

SOURCES += decoder_modplug.cpp \
    decodermodplugfactory.cpp \
    archivereader.cpp \
    modplugmetadatamodel.cpp \
    modplugsettingsdialog.cpp

FORMS += \
    modplugsettingsdialog.ui

RESOURCES = translations/translations.qrc

DEFINES += HAVE_STDINT_H \
    HAVE_INTTYPES_H

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += libmodplug
}

win32 {
    LIBS += -lmodplug
    DEFINES -= UNICODE
}
