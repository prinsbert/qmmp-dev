include(../../plugins.pri)

TARGET =$$PLUGINS_PREFIX/Visual/projectm
QMAKE_CLEAN =$$PLUGINS_PREFIX/Visual/libprojectm.so

HEADERS += projectmwidget.h \
           visualprojectmfactory.h \
           projectmplugin.h \
    projectmwrapper.h
SOURCES += projectmplugin.cpp \
           visualprojectmfactory.cpp \
           projectmwidget.cpp \
    projectmwrapper.cpp
CONFIG += warn_on \
plugin \
link_pkgconfig

TEMPLATE = lib

RESOURCES = translations/translations.qrc

unix {
  PKGCONFIG += libprojectM
  target.path = $$LIB_DIR/qmmp/Visual
  INSTALLS += target
  LIBS += -lqmmp -L/usr/lib -I/usr/include
  #projectM config path
  PROJECTM_CONFIG_FILES = /usr/share/projectM/config.inp \
                          /usr/local/share/projectM/config.inp
  for(path, PROJECTM_CONFIG_FILES) {
    exists($$path) {
      message ("found projectm configuration: "$$path)
      DEFINES += PROJECTM_CONFIG=\\\"$$path\\\"
    }
  }
}

win32 {
    LIBS += -lqmmp0 -lprojectM.dll
}
