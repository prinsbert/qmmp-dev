project(libmodplug)

INCLUDE(CheckIncludeFile)

# modplug
pkg_search_module(MODPLUG libmodplug IMPORTED_TARGET)

# add modplug defines
check_include_file(stdint.h STDINT_FOUND)
if(STDINT_FOUND)
    add_definitions(-DHAVE_STDINT_H)
endif(STDINT_FOUND)

check_include_file(inttypes.h INTTYPES_FOUND)
if(INTTYPES_FOUND)
    add_definitions(-DHAVE_INTTYPES_H)
endif(INTTYPES_FOUND)

SET(libmodplug_SRCS
    decoder_modplug.cpp
    decodermodplugfactory.cpp
    modplugmetadatamodel.cpp
    modplugsettingsdialog.cpp
    archivereader.cpp
    modplugsettingsdialog.ui
    translations/translations.qrc
)


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MODPLUG_FOUND)
    add_library(modplug MODULE ${libmodplug_SRCS})
    target_link_libraries(modplug PRIVATE Qt5::Widgets PkgConfig::QMMP PkgConfig::MODPLUG)
    install(TARGETS modplug DESTINATION ${PLUGIN_DIR}/Input)
endif(MODPLUG_FOUND)


