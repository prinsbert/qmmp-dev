include(../../plugins.pri)

HEADERS += outputjackfactory.h \
           outputjack.h \
           bio2jack.h

SOURCES += outputjackfactory.cpp \
           outputjack.cpp \
           bio2jack.c

TARGET=$$PLUGINS_PREFIX/Output/jack
QMAKE_CLEAN =$$PLUGINS_PREFIX/Output/libjack.so

CONFIG += warn_on \
thread \
plugin \
link_pkgconfig
TEMPLATE = lib
LIBS += -lqmmp
PKGCONFIG += jack soxr

RESOURCES = translations/translations.qrc

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
