include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Ui/qsui

SOURCES += \
    actionmanager.cpp \
    qsuicoverwidget.cpp \
    qsuilistwidget.cpp \
    qsuimainwindow.cpp \
    qsuipopupsettings.cpp \
    qsuipopupwidget.cpp \
    qsuipositionslider.cpp \
    shortcutitem.cpp \
    equalizer.cpp \
    logo.cpp \
    keyboardmanager.cpp \
    aboutqsuidialog.cpp \
    qsuifactory.cpp \
    qsuisettings.cpp \
    fft.c \
    qsuitabwidget.cpp \
    qsuitabbar.cpp \
    eqpreset.cpp \
    filesystembrowser.cpp \
    elidinglabel.cpp \
    playlistbrowser.cpp \
    toolbareditor.cpp \
    listwidgetdrawer.cpp \
    playlistheader.cpp \
    hotkeyeditor.cpp \
    volumeslider.cpp \
    qsuiquicksearch.cpp \
    qsuivisualization.cpp \
    qsuiwaveformseekbar.cpp \
    qsuistatusbar.cpp \
    dockwidgetlist.cpp
HEADERS += \
    actionmanager.h \
    qsuicoverwidget.h \
    qsuilistwidget.h \
    qsuimainwindow.h \
    qsuipopupsettings.h \
    qsuipopupwidget.h \
    qsuipositionslider.h \
    shortcutitem.h \
    equalizer.h \
    logo.h \
    keyboardmanager.h \
    aboutqsuidialog.h \
    qsuifactory.h \
    qsuisettings.h \
    fft.h \
    inlines.h \
    qsuitabwidget.h \
    qsuitabbar.h \
    eqpreset.h \
    filesystembrowser.h \
    elidinglabel.h \
    playlistbrowser.h \
    toolbareditor.h \
    listwidgetdrawer.h \
    playlistheader.h \
    hotkeyeditor.h \
    volumeslider.h \
    qsuiquicksearch.h \
    qsuivisualization.h \
    qsuiwaveformseekbar.h \
    qsuistatusbar.h \
    dockwidgetlist.h

FORMS += \
    forms/qsuihotkeyeditor.ui \
    forms/qsuimainwindow.ui \
    forms/qsuipopupsettings.ui \
    forms/aboutqsuidialog.ui \
    forms/qsuisettings.ui \
    forms/toolbareditor.ui

RESOURCES += translations/translations.qrc resources/qsui_resources.qrc txt/qsui_txt.qrc

LIBS += $$QMMPUI_LIB

unix {
  target.path = $$PLUGIN_DIR/Ui
  INSTALLS += target
}

win32 {
  INCLUDEPATH += ./
}
