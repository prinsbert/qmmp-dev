include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

unix:contains(CONFIG, FFVIDEO_PLUGIN):SUBDIRS += ffvideo
