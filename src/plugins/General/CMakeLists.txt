option(USE_MPRIS          "enable/disable mpris plugin" ON)
option(USE_SCROBBLER      "enable/disable scrobbler plugin" ON)
option(USE_LISTENBRAINZ   "enable/disable listenbrainz plugin" ON)
option(USE_STATICON       "enable/disable status icon plugin" ON)
option(USE_NOTIFIER       "enable/disable notifier plugin" ON)
option(USE_LYRICS         "enable/disable lyrics version" ON)
option(USE_UDISKS         "enable/disable udisks plugin" ON)
option(USE_HOTKEY         "enable/disable global hotkey plugin" ON)
option(USE_GNOMEHOTKEY    "enable/disable gnome hotkey plugin" ON)
option(USE_FILEOPS        "enable/disable fileops plugin" ON)
option(USE_COVER          "enable/disable cover manager plugin" ON)
option(USE_KDENOTIFY      "enable/disable kde notification plugin" ON)
option(USE_CONVERTER      "enable/disable audio converter plugin" ON)
option(USE_RGSCAN         "enable/disable replaygain scanner plugin" ON)
option(USE_SB             "enable/disable stream browser plugin" ON)
option(USE_TRACKCHANGE    "enable/disable track change plugin" ON)
option(USE_COPYPASTE      "enable/disable copy/paste change plugin" ON)
option(USE_HISTORY        "enable/disable history plugin" ON)
option(USE_SLEEPINHIBITOR "enable/disable sleep mode inhibition plugin" ON)
option(USE_LIBRARY        "enable/disable media library plugin")
option(USE_TASKBAR        "enable/disable taskbar plugin (Windows)" ON)
option(USE_RDETECT        "enable/disable rdetect plugin (Windows)" ON)

if(USE_MPRIS AND Qt6DBus_FOUND)
    add_subdirectory(mpris)
endif(USE_MPRIS AND Qt6DBus_FOUND)

if(USE_SCROBBLER)
    add_subdirectory(scrobbler)
endif(USE_SCROBBLER)

if(USE_LISTENBRAINZ)
    add_subdirectory(listenbrainz)
endif(USE_LISTENBRAINZ)

if(USE_STATICON)
    add_subdirectory(statusicon)
endif(USE_STATICON)

if(USE_NOTIFIER)
    add_subdirectory(notifier)
endif(USE_NOTIFIER)

if(USE_LYRICS)
    add_subdirectory(lyrics)
endif(USE_LYRICS)

if(USE_UDISKS AND Qt6DBus_FOUND)
    add_subdirectory(udisks)
endif(USE_UDISKS AND Qt6DBus_FOUND)

if(USE_HOTKEY AND X11_FOUND)
    add_subdirectory(hotkey)
endif(USE_HOTKEY AND X11_FOUND)

if(USE_FILEOPS)
    add_subdirectory(fileops)
endif(USE_FILEOPS)

if(USE_COVER)
    add_subdirectory(covermanager)
endif(USE_COVER)

if(USE_KDENOTIFY AND Qt6DBus_FOUND)
    add_subdirectory(kdenotify)
endif(USE_KDENOTIFY AND Qt6DBus_FOUND)

if(USE_CONVERTER AND TAGLIB_FOUND)
    add_subdirectory(converter)
endif(USE_CONVERTER AND TAGLIB_FOUND)

if(USE_RGSCAN AND TAGLIB_FOUND)
    add_subdirectory(rgscan)
endif(USE_RGSCAN AND TAGLIB_FOUND)

if(USE_SB)
    add_subdirectory(streambrowser)
endif(USE_SB)

if(USE_TRACKCHANGE)
    add_subdirectory(trackchange)
endif(USE_TRACKCHANGE)

if(USE_COPYPASTE)
    add_subdirectory(copypaste)
endif(USE_COPYPASTE)

if(USE_GNOMEHOTKEY AND Qt6DBus_FOUND)
    add_subdirectory(gnomehotkey)
endif(USE_GNOMEHOTKEY AND Qt6DBus_FOUND)

if(USE_HISTORY AND Qt6Sql_FOUND)
   add_subdirectory(history)
endif(USE_HISTORY AND Qt6Sql_FOUND)

if(USE_SLEEPINHIBITOR AND Qt6DBus_FOUND)
   add_subdirectory(sleepinhibitor)
endif(USE_SLEEPINHIBITOR AND Qt6DBus_FOUND)

if(USE_TASKBAR AND WIN32)
   add_subdirectory(taskbar)
endif(USE_TASKBAR AND WIN32)

if(USE_RDETECT AND WIN32)
   add_subdirectory(rdetect)
endif(USE_RDETECT AND WIN32)

if(USE_LIBRARY AND Qt6Sql_FOUND)
    add_subdirectory(library)
endif(USE_LIBRARY AND Qt6Sql_FOUND)
