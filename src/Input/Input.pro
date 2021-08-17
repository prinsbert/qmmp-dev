include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

contains(CONFIG, XMP_PLUGIN):SUBDIRS += xmp
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
