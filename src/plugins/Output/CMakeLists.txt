option(USE_ALSA         "enable/disable alsa plugin" ON)
option(USE_JACK         "enable/disable jack plugin" ON)
option(USE_OSS          "enable/disable oss plugin" OFF)
option(USE_OSS4         "enable/disable oss4 plugin" ON)
option(USE_PULSE        "enable/disable pulseaudio plugin" ON)
option(USE_PIPEWIRE     "enable/disable pipewire plugin" ON)
option(USE_NULL         "enable/disable null output plugin" ON)
option(USE_WAVEOUT      "enable/disable Win32 waveout plugin" ON)
option(USE_DSOUND       "enable/disable DirectSound plugin" ON)
option(USE_QTMULTIMEDIA "enable/disable Qt Multimedia output plugin" ON)
option(USE_WASAPI       "enable/disable WASAPI plugin" ON)
option(USE_SHOUT        "enable/disable Icecast plugin" ON)

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
