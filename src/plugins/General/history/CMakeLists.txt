project(libhistory)

set(libhistory_SRCS
    dateinputdialog.cpp
    history.cpp
    historyfactory.cpp
    historysettingsdialog.cpp
    historywindow.cpp
    progressbaritemdelegate.cpp
    historysettingsdialog.ui
    dateinputdialog.ui
    historywindow.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(history MODULE ${libhistory_SRCS})
target_link_libraries(history PRIVATE Qt5::Widgets Qt5::Sql libqmmpui libqmmp)
install(TARGETS history DESTINATION ${PLUGIN_DIR}/General)
