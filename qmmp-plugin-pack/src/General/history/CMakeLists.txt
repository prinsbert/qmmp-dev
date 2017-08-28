project(libsrconverter)

SET(QT_USE_QTSQL TRUE)
INCLUDE(FindQt4)
include(${QT_USE_FILE})

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# libqmmpui
include_directories(${QMMPUI_INCLUDE_DIRS})
link_directories( ${QMMPUI_LIBRARY_DIRS})

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

IF(QMMPUI_FOUND)
ADD_LIBRARY(history MODULE ${libhistory_SRCS} ${libhistory_UIS_H} ${libhistory_RCC_SRCS})
target_link_libraries(history ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${QMMPUI_LDFLAGS})
install(TARGETS history DESTINATION ${LIB_DIR}/qmmp/General)
ENDIF(QMMPUI_FOUND)
