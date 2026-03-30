project(libstatusicon)

set(libstatusicon_SRCS
    statusiconsettingsdialog.cpp
    statusiconfactory.cpp
    qmmptrayicon.cpp
    statusiconpopupwidget.cpp
    statusiconcoverwidget.cpp
    statusiconsettingsdialog.ui
    translations/translations.qrc
    images/tray_images.qrc
)

if(NOT WIN32)
    set(libstatusicon_SRCS statusicon.cpp ${libstatusicon_SRCS})
endif()

add_library(statusicon MODULE ${libstatusicon_SRCS})
target_link_libraries(statusicon PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS statusicon DESTINATION ${PLUGIN_DIR}/General)
