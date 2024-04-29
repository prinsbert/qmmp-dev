include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Visual/analyzer

HEADERS += analyzer.h \
           analyzercolorwidget.h \
           analyzersettingsdialog.h \
           fft.h \
           visualanalyzerfactory.h \
           inlines.h
SOURCES += analyzer.cpp \
           analyzercolorwidget.cpp \
           analyzersettingsdialog.cpp \
           fft.c \
           visualanalyzerfactory.cpp

FORMS += \
    analyzersettingsdialog.ui

RESOURCES = translations/translations.qrc

unix{
   target.path = $$PLUGIN_DIR/Visual
   INSTALLS += target
}

INCLUDEPATH += ./
