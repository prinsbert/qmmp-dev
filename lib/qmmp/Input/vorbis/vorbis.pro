# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/ogg
# ???? - ??????????:  

FORMS += detailsdialog.ui 
HEADERS += decodervorbisfactory.h \
           decoder_vorbis.h \
           detailsdialog.h \
           tag.h 
SOURCES += decoder_vorbis.cpp \
           decodervorbisfactory.cpp \
           detailsdialog.cpp \
           tag.cpp 
DESTDIR = ../
QMAKE_CLEAN += ../libvorbis.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib
PKGCONFIG += taglib ogg vorbisfile vorbis
TRANSLATIONS = translations/vorbis_plugin_ru.ts \
               translations/vorbis_plugin_uk_UA.ts \
	       translations/vorbis_plugin_zh_CN.ts \
               translations/vorbis_plugin_zh_TW.ts
RESOURCES = translations/translations.qrc
isEmpty (LIB_DIR){
LIB_DIR = /lib
}
target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
