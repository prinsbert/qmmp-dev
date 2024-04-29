include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Visual/analyzer

HEADERS += analyzer.h \
           analyzersettingsdialog.h \
           fft.h \
           visualanalyzerfactory.h \
           inlines.h \
           colorwidget.h
SOURCES += analyzer.cpp \
           analyzersettingsdialog.cpp \
           fft.c \
           visualanalyzerfactory.cpp \
           colorwidget.cpp

FORMS += \
    analyzersettingsdialog.ui

RESOURCES = translations/translations.qrc

unix{
   target.path = $$PLUGIN_DIR/Visual
   INSTALLS += target
}

INCLUDEPATH += ./
