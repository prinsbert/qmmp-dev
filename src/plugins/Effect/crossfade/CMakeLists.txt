project(libcrossfade)

SET(libcrossfade_SRCS
    crossfadeplugin.cpp
    crossfadesettingsdialog.cpp
    effectcrossfadefactory.cpp
    crossfadesettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(crossfade MODULE ${libcrossfade_SRCS})
target_link_libraries(crossfade PRIVATE Qt6::Widgets libqmmp)
install(TARGETS crossfade DESTINATION ${PLUGIN_DIR}/Effect)
