include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/General/xdghotkey

QT += dbus

HEADERS += \
    xdgglobalshortcuts.h \
    xdghotkeyfactory.h

SOURCES += \
    xdgglobalshortcuts.cpp \
    xdghotkeyfactory.cpp

#RESOURCES = translations/translations.qrc

LIBS += $$QMMPUI_LIB

target.path = $$PLUGIN_DIR/General
INSTALLS += target
