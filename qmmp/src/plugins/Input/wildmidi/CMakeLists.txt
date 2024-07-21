project(libwildmidi)

INCLUDE(CheckIncludeFile)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(libwildmidi_SRCS
  decoder_wildmidi.cpp
  decoderwildmidifactory.cpp
  wildmidisettingsdialog.cpp
  wildmidihelper.cpp
  wildmidisettingsdialog.ui
  translations/translations.qrc
)

# libwildmidi
CHECK_INCLUDE_FILE(wildmidi_lib.h WILDMIDI_HEADER_FOUND)
FIND_LIBRARY(WILDMIDI_LIB NAME WildMidi PATHS /usr/lib /usr/lib32  /usr/local/lib /usr/local/lib32)

if(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
    set(WILDMIDI_FOUND TRUE CACHE INTERNAL "wildmidi")
    message(STATUS "Found WildMidi: ${WILDMIDI_LIB}")
else(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
    message(STATUS "Could not find WildMidi library")
endif(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WILDMIDI_FOUND)
    add_library(wildmidi MODULE ${libwildmidi_SRCS})
    target_link_libraries(wildmidi Qt6::Widgets libqmmp ${WILDMIDI_LIB})
    install(TARGETS wildmidi DESTINATION ${PLUGIN_DIR}/Input)
endif(WILDMIDI_FOUND)
