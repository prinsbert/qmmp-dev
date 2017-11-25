include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/General/history

QT += sql

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

RESOURCES = translations/translations.qrc

win32:LIBS += -lqmmpui0

unix {
    PKGCONFIG += qmmpui
    target.path = $$LIB_DIR/qmmp/General
    INSTALLS += target
}
