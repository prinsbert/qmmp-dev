project(libalsa)

# alsa
pkg_search_module(ALSA alsa>=1.0.22 IMPORTED_TARGET)

SET(libalsa_SRCS
  outputalsa.cpp
  outputalsafactory.cpp
  alsasettingsdialog.cpp
  alsasettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(ALSA_FOUND)
    add_library(alsa MODULE ${libalsa_SRCS})
    target_link_libraries(alsa PRIVATE Qt6::Widgets libqmmp PkgConfig::ALSA)
    install(TARGETS alsa DESTINATION ${PLUGIN_DIR}/Output)
endif(ALSA_FOUND)

