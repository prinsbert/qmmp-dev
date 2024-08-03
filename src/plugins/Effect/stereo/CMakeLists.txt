project(libstereo)

set(libstereo_SRCS
    stereoplugin.cpp
    stereosettingsdialog.cpp
    effectstereofactory.cpp
    stereosettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(stereo MODULE ${libstereo_SRCS})
target_link_libraries(stereo Qt5::Widgets libqmmp)
install(TARGETS stereo DESTINATION ${PLUGIN_DIR}/Effect)
