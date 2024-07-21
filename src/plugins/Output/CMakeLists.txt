set(USE_ALSA TRUE CACHE BOOL "enable/disable alsa plugin")
set(USE_JACK TRUE CACHE BOOL "enable/disable jack plugin")
set(USE_OSS FALSE CACHE BOOL "enable/disable oss plugin")
set(USE_OSS4 TRUE CACHE BOOL "enable/disable oss4 plugin")
set(USE_PULSE TRUE CACHE BOOL "enable/disable pulseaudio plugin")
set(USE_PIPEWIRE TRUE CACHE BOOL "enable/disable pipewire plugin")
set(USE_NULL TRUE CACHE BOOL "enable/disable null output plugin")
set(USE_WAVEOUT TRUE CACHE BOOL "enable/disable Win32 waveout plugin")
set(USE_DSOUND TRUE CACHE BOOL "enable/disable DirectSound plugin")
set(USE_QTMULTIMEDIA TRUE CACHE BOOL "enable/disable Qt Multimedia output plugin")
set(USE_WASAPI TRUE CACHE BOOL "enable/disable WASAPI plugin")
set(USE_SHOUT TRUE CACHE BOOL "enable/disable Icecast plugin")

if(USE_ALSA)
    add_subdirectory(alsa)
endif(USE_ALSA)

if(USE_JACK)
    add_subdirectory(jack)
endif(USE_JACK)

if(USE_OSS)
    add_subdirectory(oss)
endif(USE_OSS)

if(USE_OSS4)
    add_subdirectory(oss4)
endif(USE_OSS4)

if(USE_PULSE)
    add_subdirectory(pulseaudio)
endif(USE_PULSE)

if(USE_PIPEWIRE)
    add_subdirectory(pipewire)
endif(USE_PIPEWIRE)

if(USE_NULL)
    add_subdirectory(null)
endif(USE_NULL)

if(USE_WAVEOUT)
    add_subdirectory(waveout)
endif(USE_WAVEOUT)

if(USE_DSOUND)
    add_subdirectory(directsound)
endif(USE_DSOUND)

if(USE_QTMULTIMEDIA AND Qt6Multimedia_FOUND)
    add_subdirectory(qtmultimedia)
endif(USE_QTMULTIMEDIA AND Qt6Multimedia_FOUND)

if(USE_WASAPI)
    add_subdirectory(wasapi)
endif(USE_WASAPI)

if(USE_SHOUT)
    add_subdirectory(shout)
endif(USE_SHOUT)
