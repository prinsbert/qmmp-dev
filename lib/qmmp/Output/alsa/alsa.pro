# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: ./Plugins/Output/alsa
# ???? - ??????????:  

HEADERS += outputalsa.h \
           outputalsafactory.h  \
           settingsdialog.h
SOURCES += outputalsa.cpp \
           outputalsafactory.cpp  \
           settingsdialog.cpp
INCLUDEPATH += ../../../
QMAKE_LIBDIR += ../../../
QMAKE_CLEAN = ../libalsa.so
CONFIG += release \
warn_on \
thread \
plugin 
DESTDIR = ../
TEMPLATE = lib
LIBS += -lqmmp -lasound
FORMS += settingsdialog.ui
TRANSLATIONS = translations/alsa_plugin_ru.ts \
               translations/alsa_plugin_uk_UA.ts \
	       translations/alsa_plugin_zh_CN.ts
RESOURCES = translations/translations.qrc

isEmpty (LIB_DIR){
LIB_DIR = /lib
}

target.path = $$LIB_DIR/qmmp/Output
INSTALLS += target
