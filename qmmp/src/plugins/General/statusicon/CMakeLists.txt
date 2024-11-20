project(libstatusicon)

set(libstatusicon_SRCS
    statusiconsettingsdialog.cpp
    statusicon.cpp
    statusiconfactory.cpp
    qmmptrayicon.cpp
    statusiconpopupwidget.cpp
    statusiconcoverwidget.cpp
    statusiconsettingsdialog.ui
    translations/translations.qrc
    images/tray_images.qrc
)

add_library(statusicon MODULE ${libstatusicon_SRCS})
target_link_libraries(statusicon PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS statusicon DESTINATION ${PLUGIN_DIR}/General)
