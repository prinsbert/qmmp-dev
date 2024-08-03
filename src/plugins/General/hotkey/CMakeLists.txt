project(libhotkey)

include(CheckIncludeFile)

# xcb
pkg_search_module(XCB xcb IMPORTED_TARGET)

SET(libhotkey_SRCS
    hotkeysettingsdialog.cpp
    hotkeyfactory.cpp
    hotkeymanager_x11.cpp
    hotkeymanager_win.cpp
    hotkeydialog.cpp
    hotkeysettingsdialog.ui
    hotkeydialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(X11_FOUND AND XCB_FOUND)
    set_property(SOURCE hotkeymanager_x11.cpp PROPERTY SKIP_AUTOMOC ON)
    add_library(hotkey MODULE ${libhotkey_SRCS})
    target_link_libraries(hotkey PRIVATE Qt5::Widgets Qt5::X11Extras libqmmpui libqmmp PkgConfig::X11 PkgConfig::XCB)
    install(TARGETS hotkey DESTINATION ${PLUGIN_DIR}/General)
endif(X11_FOUND AND XCB_FOUND)
