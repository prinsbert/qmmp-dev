project(libtwopanelfiledialog)

set(libtwopanelfiledialog_SRCS
    twopanelfiledialogimpl.cpp
    twopanelfiledialog.cpp
    twopanelfiledialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(twopanelfiledialog MODULE ${libtwopanelfiledialog_SRCS})
target_link_libraries(twopanelfiledialog PRIVATE Qt5::Widgets libqmmpui libqmmp)
install(TARGETS twopanelfiledialog DESTINATION ${PLUGIN_DIR}/FileDialogs)
