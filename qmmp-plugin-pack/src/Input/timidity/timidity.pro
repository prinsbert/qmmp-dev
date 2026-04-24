include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/timidity

HEADERS += decodertimidityfactory.h \
    decoder_timidity.h \
    timidityhelper.h \
    timiditysettingsdialog.h

SOURCES += decoder_timidity.cpp \
    decodertimidityfactory.cpp \
    timidityhelper.cpp \
    timiditysettingsdialog.cpp

FORMS += \
    timiditysettingsdialog.ui

#RESOURCES = translations/translations.qrc

PKGCONFIG += libtimidity

target.path = $$PLUGIN_DIR/Input
INSTALLS += target
