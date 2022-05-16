include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

contains(CONFIG, MODPLUG_PLUGIN):SUBDIRS += modplug
contains(CONFIG, FFAP_PLUGIN):SUBDIRS += ffap
