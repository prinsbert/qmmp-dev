include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

unix:contains(CONFIG, MODPLUG_PLUGIN):SUBDIRS += modplug
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
