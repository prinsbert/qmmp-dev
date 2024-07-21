project(liblibrary)

set(liblibrary_SRCS
    library.cpp
    libraryfactory.cpp
    librarymodel.cpp
    librarywidget.cpp
    librarysettingsdialog.cpp
    librarywidget.ui
    librarysettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(library MODULE ${liblibrary_SRCS})
target_link_libraries(library PRIVATE Qt6::Widgets Qt6::Sql libqmmpui libqmmp)
install(TARGETS library DESTINATION ${PLUGIN_DIR}/General)
