include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Ui/qsui

SOURCES += \
    qsuiactionmanager.cpp \
    qsuicoverwidget.cpp \
    qsuihotkeyeditor.cpp \
    qsuikeyboardmanager.cpp \
    qsuilistwidget.cpp \
    qsuilistwidgetdrawer.cpp \
    qsuilogo.cpp \
    qsuimainwindow.cpp \
    qsuiplaylistheader.cpp \
    qsuipopupsettings.cpp \
    qsuipopupwidget.cpp \
    qsuipositionslider.cpp \
    shortcutitem.cpp \
    equalizer.cpp \
    aboutqsuidialog.cpp \
    qsuifactory.cpp \
    qsuisettings.cpp \
    fft.c \
    qsuitabwidget.cpp \
    qsuitabbar.cpp \
    filesystembrowser.cpp \
    elidinglabel.cpp \
    playlistbrowser.cpp \
    toolbareditor.cpp \
    volumeslider.cpp \
    qsuiquicksearch.cpp \
    qsuivisualization.cpp \
    qsuiwaveformseekbar.cpp \
    qsuistatusbar.cpp \
    dockwidgetlist.cpp
HEADERS += \
    qsuiactionmanager.h \
    qsuicoverwidget.h \
    qsuihotkeyeditor.h \
    qsuikeyboardmanager.h \
    qsuilistwidget.h \
    qsuilistwidgetdrawer.h \
    qsuilogo.h \
    qsuimainwindow.h \
    qsuiplaylistheader.h \
    qsuipopupsettings.h \
    qsuipopupwidget.h \
    qsuipositionslider.h \
    shortcutitem.h \
    equalizer.h \
    aboutqsuidialog.h \
    qsuifactory.h \
    qsuisettings.h \
    fft.h \
    inlines.h \
    qsuitabwidget.h \
    qsuitabbar.h \
    filesystembrowser.h \
    elidinglabel.h \
    playlistbrowser.h \
    toolbareditor.h \
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
