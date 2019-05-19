include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

contains(CONFIG, YOUTUBE_PLUGIN):SUBDIRS += ytb
