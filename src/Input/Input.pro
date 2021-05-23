include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

unix:contains(CONFIG, XMP_PLUGIN):SUBDIRS += xmp
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
