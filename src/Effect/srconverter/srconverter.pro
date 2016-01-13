include(../../plugins.pri)

HEADERS += srconverter.h \
 effectsrconverterfactory.h \
 settingsdialog.h

SOURCES += srconverter.cpp \
 effectsrconverterfactory.cpp \
 settingsdialog.cpp

TARGET=$$PLUGINS_PREFIX/Effect/srconverter
QMAKE_CLEAN =$$PLUGINS_PREFIX/Effect/libsrconverter.so
INCLUDEPATH += ../../../
CONFIG += warn_on \
plugin \
link_pkgconfig

PKGCONFIG += qmmp samplerate
TEMPLATE = lib


RESOURCES = translations/translations.qrc

isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target

FORMS += settingsdialog.ui

