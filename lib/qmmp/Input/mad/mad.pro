# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Input/mad
# ???? - ??????????:  

FORMS += detailsdialog.ui \
         settingsdialog.ui 
HEADERS += decodermadfactory.h \
           decoder_mad.h \
           detailsdialog.h \
           settingsdialog.h \
           id3tag.h 
SOURCES += decoder_mad.cpp \
           decodermadfactory.cpp \
           detailsdialog.cpp \
           settingsdialog.cpp \
           id3tag.cpp 
DESTDIR = ../
QMAKE_CLEAN += ../libmad.so
INCLUDEPATH += ../../../
CONFIG += release \
warn_on \
plugin \
link_pkgconfig
TEMPLATE = lib
QMAKE_LIBDIR += ../../../
LIBS += -lqmmp
PKGCONFIG += taglib mad
TRANSLATIONS = translations/mad_plugin_ru.ts \
               translations/mad_plugin_uk_UA.ts \
	       translations/mad_plugin_zh_CN.ts \
	       translations/mad_plugin_zh_TW.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Input
INSTALLS += target
