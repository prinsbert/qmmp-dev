include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/mpg123

HEADERS += decodermpg123factory.h \
    decoder_mpg123.h \
    settingsdialog.h \
    tagextractor.h \
    mpegmetadatamodel.h \
    replaygainreader.h

SOURCES += decoder_mpg123.cpp \
    decodermpg123factory.cpp \
    settingsdialog.cpp \
    tagextractor.cpp \
    mpegmetadatamodel.cpp \
    replaygainreader.cpp

FORMS += settingsdialog.ui

RESOURCES = translations/translations.qrc

unix {
    PKGCONFIG += taglib libmpg123
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
