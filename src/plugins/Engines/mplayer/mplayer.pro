include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Engines/mplayer

HEADERS += mplayerenginefactory.h \
    mplayerengine.h \
    mplayermetadatamodel.h \
    mplayersettingsdialog.h

SOURCES += mplayerengine.cpp \
    mplayerenginefactory.cpp \
    mplayermetadatamodel.cpp \
    mplayersettingsdialog.cpp

FORMS += \
    mplayersettingsdialog.ui

RESOURCES = translations/translations.qrc

target.path = $$PLUGIN_DIR/Engines
INSTALLS += target
