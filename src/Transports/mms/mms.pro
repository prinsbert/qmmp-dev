include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Transports/mms

HEADERS += \
    mmsinputfactory.h \
    mmsinputsource.h \
    mmssettingsdialog.h \
    mmsstreamreader.h

SOURCES += \
    mmsinputfactory.cpp \
    mmsinputsource.cpp \
    mmssettingsdialog.cpp \
    mmsstreamreader.cpp

FORMS += \
    mmssettingsdialog.ui

RESOURCES = translations/translations.qrc

LIBS += -L/usr/lib
PKGCONFIG += libmms

target.path = $$PLUGIN_DIR/Transports
INSTALLS += target

QMAKE_CFLAGS_ISYSTEM='' #removes "-isystem /usr/include"  provided by libmms.pc
