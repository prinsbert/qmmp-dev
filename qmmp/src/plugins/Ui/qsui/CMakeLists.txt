project(qsui)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


SET(libqsui_SRCS
    aboutqsuidialog.cpp
    qsuiactionmanager.cpp
    dockwidgetlist.cpp
    elidinglabel.cpp
    qsuiequalizer.cpp
    filesystembrowser.cpp
    qsuikeyboardmanager.cpp
    qsuiplaylistbrowser.cpp
    qsuicoverwidget.cpp
    qsuifactory.cpp
    qsuihotkeyeditor.cpp
    qsuilistwidget.cpp
    qsuilistwidgetdrawer.cpp
    qsuilogo.cpp
    qsuimainwindow.cpp
    qsuiplaylistheader.cpp
    qsuipopupsettings.cpp
    qsuipopupwidget.cpp
    qsuipositionslider.cpp
    qsuiquicksearch.cpp
    qsuisettings.cpp
    qsuistatusbar.cpp
    qsuitabbar.cpp
    qsuitabwidget.cpp
    qsuivisualization.cpp
    qsuiwaveformseekbar.cpp
    qsuishortcutitem.cpp
    toolbareditor.cpp
    volumeslider.cpp
    fft.c
    forms/qsuimainwindow.ui
    forms/qsuipopupsettings.ui
    forms/qsuisettings.ui
    forms/aboutqsuidialog.ui
    forms/toolbareditor.ui
    forms/qsuihotkeyeditor.ui
    translations/translations.qrc
    txt/qsui_txt.qrc
    resources/qsui_resources.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/forms)

ADD_LIBRARY(qsui MODULE ${libqsui_SRCS})
add_dependencies(qsui libqmmpui libqmmp)
target_link_libraries(qsui PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS qsui DESTINATION ${PLUGIN_DIR}/Ui)
