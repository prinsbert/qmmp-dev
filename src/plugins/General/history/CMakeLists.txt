project(libhistory)

SET(QT_USE_QTSQL TRUE)
INCLUDE(FindQt4)
include(${QT_USE_FILE})

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

ADD_DEFINITIONS(${SAMPLERATE_CFLAGS})

SET(libhistory_SRCS
    dateinputdialog.cpp
    history.cpp
    historyfactory.cpp
    historysettingsdialog.cpp
    historywindow.cpp
    progressbaritemdelegate.cpp
)

SET(libhistory_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libhistory_RCC_SRCS ${libhistory_RCCS})

# user interface
SET(libhistory_UIS
    historysettingsdialog.ui
    dateinputdialog.ui
    historywindow.ui
)

QT4_WRAP_UI(libhistory_UIS_H ${libhistory_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(history MODULE ${libhistory_SRCS} ${libhistory_UIS_H} ${libhistory_RCC_SRCS})
target_link_libraries(history ${QT_LIBRARIES} libqmmpui libqmmp)
install(TARGETS history DESTINATION ${PLUGIN_DIR}/General)
