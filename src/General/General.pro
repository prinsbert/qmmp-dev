include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

unix {
contains(CONFIG, HISTORY_PLUGIN):SUBDIRS += history
}



