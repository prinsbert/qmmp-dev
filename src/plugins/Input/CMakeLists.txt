option(USE_MAD     "enable/disable mad decoder (mpeg plugin)" ON)
option(USE_MPG123  "enable/disable mpg123 decoder (mpeg plugin)" ON)
option(USE_FLAC    "enable/disable flac plugin" ON)
option(USE_VORBIS  "enable/disable ogg vorbis plugin" ON)
option(USE_FFMPEG  "enable/disable ffmpeg plugin" ON)
option(USE_MPC     "enable/disable mpc plugin" ON)
option(USE_SNDFILE "enable/disable sndfile plugin" ON)
option(USE_WAVPACK "enable/disable wavpack plugin" ON)
option(USE_XMP     "enable/disable xmp plugin" ON)
option(USE_AAC     "enable/disable aac plugin" ON)
option(USE_CUE     "enable/disable cue plugin" ON)
option(USE_CDA     "enable/disable cd audio support" ON)
option(USE_MIDI    "enable/disable midi support" ON)
option(USE_GME     "enable/disable game music plugin" ON)
option(USE_OPUS    "enable/disable opus plugin" ON)
option(USE_SID     "enable/disable sid plugin" ON)
option(USE_ARCHIVE "enable/disable archive plugin" ON)


if((USE_MAD OR USE_MPG123) AND TAGLIB_FOUND)
    add_subdirectory(mpeg)
endif((USE_MAD OR USE_MPG123) AND TAGLIB_FOUND)

if(USE_FLAC AND TAGLIB_FOUND)
    add_subdirectory(flac)
endif(USE_FLAC AND TAGLIB_FOUND)

if(USE_VORBIS AND TAGLIB_FOUND)
    add_subdirectory(vorbis)
endif(USE_VORBIS AND TAGLIB_FOUND)

if(USE_FFMPEG AND TAGLIB_FOUND)
    add_subdirectory(ffmpeg)
endif(USE_FFMPEG AND TAGLIB_FOUND)

if(USE_MPC AND TAGLIB_FOUND)
    add_subdirectory(mpc)
endif(USE_MPC AND TAGLIB_FOUND)

if(USE_SNDFILE)
   add_subdirectory(sndfile)
endif(USE_SNDFILE)

if(USE_WAVPACK)
    add_subdirectory(wavpack)
endif(USE_WAVPACK)

if(USE_XMP)
   add_subdirectory(xmp)
endif(USE_XMP)

if(USE_AAC AND TAGLIB_FOUND)
   add_subdirectory(aac)
endif(USE_AAC AND TAGLIB_FOUND)

if(USE_CUE)
add_subdirectory(cue)
endif(USE_CUE)

if(USE_CDA)
add_subdirectory(cdaudio)
endif(USE_CDA)

if(USE_MIDI)
add_subdirectory(wildmidi)
endif(USE_MIDI)

if(USE_GME)
    add_subdirectory(gme)
endif(USE_GME)

if(USE_OPUS AND TAGLIB_FOUND)
    add_subdirectory(opus)
endif(USE_OPUS AND TAGLIB_FOUND)

if(USE_SID)
    add_subdirectory(sid)
endif(USE_SID)

if(USE_ARCHIVE AND TAGLIB_FOUND)
    add_subdirectory(archive)
endif(USE_ARCHIVE AND TAGLIB_FOUND)
