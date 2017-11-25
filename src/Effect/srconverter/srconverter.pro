include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Effect/srconverter

HEADERS += srconverter.h \
 effectsrconverterfactory.h \
 settingsdialog.h

SOURCES += srconverter.cpp \
 effectsrconverterfactory.cpp \
 settingsdialog.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

PKGCONFIG += qmmp samplerate

target.path = $$LIB_DIR/qmmp/Effect
INSTALLS += target
