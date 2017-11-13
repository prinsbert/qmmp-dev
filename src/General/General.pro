include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

contains(CONFIG, HISTORY_PLUGIN):SUBDIRS += history
