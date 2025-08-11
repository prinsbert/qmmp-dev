include(../../qmmp-plugin-pack.pri)

TEMPLATE = subdirs

unix {
   contains(CONFIG, FFVIDEO_PLUGIN):SUBDIRS += ffvideo
   contains(CONFIG, MPLAYER_PLUGIN):SUBDIRS += mplayer
   contains(CONFIG, MPV_PLUGIN):SUBDIRS += mpv
}
