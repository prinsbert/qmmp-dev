include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/srconverter

HEADERS += srconverter.h \
 effectsrconverterfactory.h \
 srconvertersettingsdialog.h

SOURCES += srconverter.cpp \
 effectsrconverterfactory.cpp \
 srconvertersettingsdialog.cpp

FORMS += \
    srconvertersettingsdialog.ui

RESOURCES = translations/translations.qrc

PKGCONFIG += samplerate

target.path = $$PLUGIN_DIR/Effect
INSTALLS += target
