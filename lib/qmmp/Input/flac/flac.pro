# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/flac
# ???? - ??????????:  

FORMS += detailsdialog.ui 
HEADERS += tag.h \
	   decoderflacfactory.h \
           decoder_flac.h \
           detailsdialog.h 
SOURCES += decoder_flac.cpp \
           decoderflacfactory.cpp \
           detailsdialog.cpp \
           tag.cpp 
DESTDIR = ../
QMAKE_CLEAN += ../libflac.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp -L/usr/lib -I/usr/include
PKGCONFIG += taglib flac
TRANSLATIONS = translations/flac_plugin_ru.ts \
               translations/flac_plugin_uk_UA.ts \
	       translations/flac_plugin_zh_CN.ts \
               translations/flac_plugin_zh_TW.ts \
               translations/flac_plugin_cs.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
