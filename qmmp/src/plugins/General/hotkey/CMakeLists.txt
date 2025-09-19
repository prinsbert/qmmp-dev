project(libhotkey)

# xcb
if(NOT WIN32)
    pkg_search_module(XCB xcb IMPORTED_TARGET)
endif(NOT WIN32)

SET(libhotkey_SRCS
    hotkeysettingsdialog.cpp
    hotkeyfactory.cpp
    hotkeydialog.cpp
    hotkeysettingsdialog.ui
    hotkeydialog.ui
    translations/translations.qrc
)

if(XCB_FOUND)
    set(libhotkey_SRCS ${libhotkey_SRCS} hotkeymanager_x11.cpp)
elseif(WIN32)
    set(libhotkey_SRCS ${libhotkey_SRCS} hotkeymanager_win.cpp)
endif(WIN32)

if(XCB_FOUND OR WIN32)
    set_property(SOURCE hotkeymanager_x11.cpp PROPERTY SKIP_AUTOMOC ON)
    add_library(hotkey MODULE ${libhotkey_SRCS})
    target_link_libraries(hotkey PRIVATE Qt6::Widgets libqmmpui libqmmp)
    if(XCB_FOUND)
        target_link_libraries(hotkey PRIVATE PkgConfig::X11 PkgConfig::XCB)
    endif(XCB_FOUND)
    install(TARGETS hotkey DESTINATION ${PLUGIN_DIR}/General)
endif(XCB_FOUND OR WIN32)
