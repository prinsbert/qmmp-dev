project(libsrconverter)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# libqmmpui
include_directories(${QMMPUI_INCLUDE_DIRS})
link_directories( ${QMMPUI_LIBRARY_DIRS})

SET(libhistory_SRCS
    dateinputdialog.cpp
    history.cpp
    historyfactory.cpp
    historysettingsdialog.cpp
    historywindow.cpp
    progressbaritemdelegate.cpp
)

SET(libhistory_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libhistory_RCC_SRCS ${libhistory_RCCS})

# user interface
SET(libhistory_UIS
    historysettingsdialog.ui
    dateinputdialog.ui
    historywindow.ui
)

QT5_WRAP_UI(libhistory_UIS_H ${libhistory_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(QMMPUI_FOUND)
ADD_LIBRARY(history MODULE ${libhistory_SRCS} ${libhistory_UIS_H} ${libhistory_RCC_SRCS})
target_link_libraries(history Qt5::Widgets Qt5::Sql ${QMMP_LDFLAGS} ${QMMPUI_LDFLAGS})
install(TARGETS history DESTINATION ${LIB_DIR}/qmmp/General)
ENDIF(QMMPUI_FOUND)
