include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

contains(CONFIG, GOOM_PLUGIN):SUBDIRS += goom
