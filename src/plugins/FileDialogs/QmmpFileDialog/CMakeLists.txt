project(libqmmpfiledialog)

set(libqmmpfiledialog_SRCS
    qmmpfiledialogimpl.cpp
    qmmpfiledialog.cpp
    qmmpfiledialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(qmmpfiledialog MODULE ${libqmmpfiledialog_SRCS})
target_link_libraries(qmmpfiledialog PRIVATE Qt5::Widgets libqmmpui libqmmp)
install(TARGETS qmmpfiledialog DESTINATION ${PLUGIN_DIR}/FileDialogs)
