project(libudisks)

set(libudisks_SRCS
    udisksfactory.cpp
    udisksmanager.cpp
    udisksplugin.cpp
    udisksdevice.cpp
    udiskssettingsdialog.cpp
    udiskssettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(udisks MODULE ${libudisks_SRCS})
add_dependencies(udisks libqmmpui)
target_link_libraries(udisks PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS udisks DESTINATION ${PLUGIN_DIR}/General)
