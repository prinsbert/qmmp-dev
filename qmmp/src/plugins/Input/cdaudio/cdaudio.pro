include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/cdaudio

HEADERS += decodercdaudiofactory.h \
           cdaudiosettingsdialog.h \
           decoder_cdaudio.h

SOURCES += decoder_cdaudio.cpp \
           cdaudiosettingsdialog.cpp \
           decodercdaudiofactory.cpp

FORMS += \
    cdaudiosettingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
  LIBS += -L/usr/lib  -I/usr/include
  PKGCONFIG += libcdio libcdio_cdda libcdio_paranoia libcddb
  target.path = $$PLUGIN_DIR/Input
  INSTALLS += target
}

win32 {
  LIBS += -lcdio -lcdio_paranoia -lcdio_cdda  -lm -lwinmm -mwindows -liconv -lcddb -lws2_32
}
