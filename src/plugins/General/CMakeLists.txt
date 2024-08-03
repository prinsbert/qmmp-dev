set(USE_MPRIS TRUE CACHE BOOL "enable/disable mpris plugin")
set(USE_SCROBBLER TRUE CACHE BOOL "enable/disable scrobbler plugin")
set(USE_LISTENBRAINZ TRUE CACHE BOOL "enable/disable listenbrainz plugin")
set(USE_STATICON TRUE CACHE BOOL "enable/disable status icon plugin")
set(USE_NOTIFIER TRUE CACHE BOOL "enable/disable notifier plugin")
set(USE_LYRICS TRUE CACHE BOOL "enable/disable lyrics version")
set(USE_UDISKS TRUE CACHE BOOL "enable/disable udisks plugin")
set(USE_HOTKEY TRUE CACHE BOOL "enable/disable global hotkey plugin")
set(USE_GNOMEHOTKEY TRUE CACHE BOOL "enable/disable gnome hotkey plugin")
set(USE_FILEOPS TRUE CACHE BOOL "enable/disable fileops plugin")
set(USE_COVER TRUE CACHE BOOL "enable/disable cover manager plugin")
set(USE_KDENOTIFY TRUE CACHE BOOL "enable/disable kde notification plugin")
set(USE_CONVERTER TRUE CACHE BOOL "enable/disable audio converter plugin")
set(USE_RGSCAN TRUE CACHE BOOL "enable/disable replaygain scanner plugin")
set(USE_SB TRUE CACHE BOOL "enable/disable stream browser plugin")
set(USE_TRACKCHANGE TRUE CACHE BOOL "enable/disable track change plugin")
set(USE_COPYPASTE TRUE CACHE BOOL "enable/disable copy/paste change plugin")
set(USE_HISTORY TRUE CACHE BOOL "enable/disable history plugin")
set(USE_SLEEPINHIBITOR TRUE CACHE BOOL "enable/disable sleep mode inhibition plugin")
set(USE_LIBRARY TRUE CACHE BOOL "enable/disable media library plugin")
set(USE_TASKBAR TRUE CACHE BOOL "enable/disable taskbar plugin (Windows)")
set(USE_RDETECT TRUE CACHE BOOL "enable/disable rdetect plugin (Windows)")

if(USE_MPRIS AND Qt5DBus_FOUND)
    add_subdirectory(mpris)
endif(USE_MPRIS AND Qt5DBus_FOUND)

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

if(USE_UDISKS AND Qt5DBus_FOUND)
    add_subdirectory(udisks)
endif(USE_UDISKS AND Qt5DBus_FOUND)

if(USE_HOTKEY AND X11_FOUND)
    add_subdirectory(hotkey)
endif(USE_HOTKEY AND X11_FOUND)

if(USE_FILEOPS)
    add_subdirectory(fileops)
endif(USE_FILEOPS)

if(USE_COVER)
    add_subdirectory(covermanager)
endif(USE_COVER)

if(USE_KDENOTIFY AND Qt5DBus_FOUND)
    add_subdirectory(kdenotify)
endif(USE_KDENOTIFY AND Qt5DBus_FOUND)

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

if(USE_GNOMEHOTKEY AND Qt5DBus_FOUND)
    add_subdirectory(gnomehotkey)
endif(USE_GNOMEHOTKEY AND Qt5DBus_FOUND)

if(USE_HISTORY AND Qt5Sql_FOUND)
   add_subdirectory(history)
endif(USE_HISTORY AND Qt5Sql_FOUND)

if(USE_SLEEPINHIBITOR AND Qt5DBus_FOUND)
   add_subdirectory(sleepinhibitor)
endif(USE_SLEEPINHIBITOR AND Qt5DBus_FOUND)

if(USE_TASKBAR AND WIN32)
   add_subdirectory(taskbar)
endif(USE_TASKBAR AND WIN32)

if(USE_RDETECT AND WIN32)
   add_subdirectory(rdetect)
endif(USE_RDETECT AND WIN32)

if(USE_LIBRARY AND Qt5Sql_FOUND)
    add_subdirectory(library)
endif(USE_LIBRARY AND Qt5Sql_FOUND)
