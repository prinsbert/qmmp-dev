project(libdirectsound)

include(CheckIncludeFile)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Win32 DirectSound
check_include_file(dsound.h DSOUND_H_FOUND)

SET(libdirectsound_SRCS
  outputdirectsound.cpp
  outputdirectsoundfactory.cpp
  translations/translations.qrc
)

if(DSOUND_H_FOUND)
    add_library(directsound MODULE ${libdirectsound_SRCS})
    target_link_libraries(directsound PRIVATE Qt5::Widgets libqmmp -ldxguid -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -ldsound)
    install(TARGETS directsound DESTINATION ${PLUGIN_DIR}/Output)
endif(DSOUND_H_FOUND)

