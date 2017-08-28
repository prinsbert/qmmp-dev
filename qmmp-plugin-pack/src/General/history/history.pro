include(../../plugins.pri)

CONFIG += warn_on \
plugin

QT += sql

TARGET =$$PLUGINS_PREFIX/General/history
unix : QMAKE_CLEAN = $$PLUGINS_PREFIX/General/libhistory.so

TEMPLATE = lib
unix:LIBS += -lqmmpui -lqmmp

win32:LIBS += -lqmmpui0 -lqmmp0

RESOURCES = translations/translations.qrc
isEmpty(LIB_DIR){
    LIB_DIR = /lib
}
unix {
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
HEADERS += historyfactory.h \
    history.h \
    historywindow.h \
    dateinputdialog.h \
    historysettingsdialog.h \
    progressbaritemdelegate.h

SOURCES += historyfactory.cpp \
    history.cpp \
    historywindow.cpp \
    dateinputdialog.cpp \
    historysettingsdialog.cpp \
    progressbaritemdelegate.cpp

FORMS += \
    historywindow.ui \
    dateinputdialog.ui \
    historysettingsdialog.ui

