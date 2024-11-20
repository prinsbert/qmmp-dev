project(libwildmidi)

include(CheckIncludeFile)

set(libwildmidi_SRCS
  decoder_wildmidi.cpp
  decoderwildmidifactory.cpp
  wildmidisettingsdialog.cpp
  wildmidihelper.cpp
  wildmidisettingsdialog.ui
  translations/translations.qrc
)

# libwildmidi
check_include_file(wildmidi_lib.h WILDMIDI_HEADER_FOUND)
find_library(WILDMIDI_LIB NAME WildMidi PATHS /usr/lib /usr/lib32  /usr/local/lib /usr/local/lib32)

if(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
    set(WILDMIDI_FOUND TRUE CACHE INTERNAL "wildmidi")
    message(STATUS "Found WildMidi: ${WILDMIDI_LIB}")
else(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))
    message(STATUS "Could not find WildMidi library")
endif(WILDMIDI_HEADER_FOUND AND (NOT WILDMIDI_LIB MATCHES "^.*-NOTFOUND"))

if(WILDMIDI_FOUND)
    add_library(wildmidi MODULE ${libwildmidi_SRCS})
    target_link_libraries(wildmidi Qt6::Widgets libqmmp ${WILDMIDI_LIB})
    install(TARGETS wildmidi DESTINATION ${PLUGIN_DIR}/Input)
endif(WILDMIDI_FOUND)
