include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

unix:contains(CONFIG, YOUTUBE_PLUGIN):SUBDIRS += ytb
