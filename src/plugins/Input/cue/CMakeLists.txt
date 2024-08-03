project(libcue)

if(USE_ENCA AND ENCA_FOUND)
    add_definitions(-DWITH_ENCA)
endif(USE_ENCA AND ENCA_FOUND)

SET(libcue_SRCS
  decoder_cue.cpp
  decodercuefactory.cpp
  cuesettingsdialog.cpp
  cuefile.cpp
  cuemetadatamodel.cpp
  translations/translations.qrc
  cuesettingsdialog.ui
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(cue MODULE ${libcue_SRCS} ${libcue_RCC_SRCS})
if(USE_ENCA AND ENCA_FOUND)
    target_link_libraries(cue PRIVATE Qt5::Widgets libqmmp PkgConfig::ENCA)
else(USE_ENCA AND ENCA_FOUND)
    target_link_libraries(cue PRIVATE Qt5::Widgets libqmmp)
endif(USE_ENCA AND ENCA_FOUND)
install(TARGETS cue DESTINATION ${PLUGIN_DIR}/Input)
