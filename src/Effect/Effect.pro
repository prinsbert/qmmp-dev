include (../../qmmp-plugin-pack.pri)
TEMPLATE = subdirs

unix {
contains(CONFIG, SRC_PLUGIN):SUBDIRS += srconverter
}



