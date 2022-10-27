project(libprojectm)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# projectM

IF(QT_FEATURE_opengl)
  MESSAGE(STATUS "Found desktop OpenGL implementation")
  pkg_search_module(PROJECTM libprojectM>=2.0.0)
  IF(PROJECTM_FOUND)
    ADD_DEFINITIONS(${PROJECTM_CFLAGS})
    pkg_search_module(PROJECTM_31 libprojectM>=3.1.0)
    find_file(PROJECTM_CONFIG config.inp
              PATHS "/usr/share/projectM/" "/usr/local/share/projectM/")    
  ENDIF(PROJECTM_FOUND)
ELSE(QT_FEATURE_opengl)
  MESSAGE(WARNING "Unsuppoted OpenGL implementation, projectM disabled")
ENDIF(QT_FEATURE_opengl)

IF(PROJECTM_31_FOUND)
  ADD_DEFINITIONS(-DPROJECTM_31)
ENDIF(PROJECTM_31_FOUND)

IF(PROJECTM_CONFIG)
  SET(libprojectm_DEFINES PROJECTM_CONFIG=\"${PROJECTM_CONFIG}\")
  MESSAGE(STATUS "Found projectM configuration: ${PROJECTM_CONFIG}")
ENDIF(PROJECTM_CONFIG)

SET(libprojectm_SRCS
    projectmwidget.cpp
    projectmplugin.cpp
    visualprojectmfactory.cpp
    projectmwrapper.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(PROJECTM_FOUND)
  ADD_LIBRARY(projectm MODULE ${libprojectm_SRCS})
  target_compile_definitions(projectm PRIVATE ${libprojectm_DEFINES})
  add_dependencies(projectm libqmmp)
  target_link_libraries(projectm PRIVATE Qt6::OpenGLWidgets libqmmp ${PROJECTM_LDFLAGS} -lGL)
  install(TARGETS projectm DESTINATION ${PLUGIN_DIR}/Visual)
ENDIF(PROJECTM_FOUND)
