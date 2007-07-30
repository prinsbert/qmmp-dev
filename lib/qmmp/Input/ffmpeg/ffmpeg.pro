FORMS += detailsdialog.ui 
HEADERS += decoderffmpegfactory.h \
           tag.h \
	   detailsdialog.h \
           decoder_ffmpeg.h
SOURCES += tag.cpp \
	   detailsdialog.cpp \
           decoder_ffmpeg.cpp \
	   decoderffmpegfactory.cpp
DESTDIR = ../
QMAKE_CLEAN = ../libffmpeg.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include
DEFINES += __STDC_CONSTANT_MACROS
PKGCONFIG += libavcodec libavformat
TRANSLATIONS = translations/ffmpeg_plugin_ru.ts \
               translations/ffmpeg_plugin_uk_UA.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
