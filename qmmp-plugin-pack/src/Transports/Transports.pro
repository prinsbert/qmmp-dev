include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

unix:contains(CONFIG, YOUTUBE_PLUGIN):SUBDIRS += ytb
unix:contains(CONFIG, MMS_PLUGIN):SUBDIRS += mms
