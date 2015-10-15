project(libffap)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

REMOVE_DEFINITIONS(-Wall)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})


include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libffap_SRCS
  decoder_ffap.cpp
  decoder_ffapcue.cpp
  decoderffapfactory.cpp
  ffapmetadatamodel.cpp
  cueparser.cpp
  replaygainreader.cpp
  ffap.c
)

SET(libffap_HDRS
  decoder_ffap.h
  decoder_ffapcue.h
  cueparser.h
  replaygainreader.h
  ffap.h
)

IF(USE_ASM)
FIND_PROGRAM(YASM_PATH yasm)
ENDIF(USE_ASM)

IF(USE_ASM AND (NOT YASM_PATH MATCHES "^.*-NOTFOUND"))
  SET(YASM_FOUND TRUE CACHE INTERNAL "yasm_found")
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")
   
    ADD_DEFINITIONS(-DHAVE_SSE2)
    SET(YASM_OBJS x86inc.o dsputil_yasm.o)
    
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
      ADD_DEFINITIONS(-DARCH_X86_32)
      
      add_custom_command(OUTPUT x86inc.o 
                         COMMAND yasm ARGS -m x86 -f elf32 -DARCH_X86_32 x86inc.asm -o ${CMAKE_CURRENT_BINARY_DIR}/x86inc.o
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMENT "Building x86inc.o")
      add_custom_command(OUTPUT dsputil_yasm.o
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                         COMMAND yasm ARGS -m x86 -f elf32 -DARCH_X86_32 dsputil_yasm.asm -o ${CMAKE_CURRENT_BINARY_DIR}/dsputil_yasm.o
                         DEPENDS x86inc.o COMMENT "Building dsputil_yasm.o")
      
      message("-- Architecture: x86")
    ELSEIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      ADD_DEFINITIONS(-DARCH_X86_64)
      
      add_custom_command(OUTPUT x86inc.o 
                         COMMAND yasm ARGS -m amd64 -f elf64 -DARCH_X86_64 -DPIC x86inc.asm -o ${CMAKE_CURRENT_BINARY_DIR}/x86inc.o
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMENT "compiling  x86inc.asm")
      add_custom_command(OUTPUT dsputil_yasm.o
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                         COMMAND yasm ARGS -m amd64 -f elf64 -DARCH_X86_64 -DPIC dsputil_yasm.asm -o ${CMAKE_CURRENT_BINARY_DIR}/dsputil_yasm.o
                         DEPENDS x86inc.o COMMENT "compiling  dsputil_yasm.asm")
      
      message("-- Architecture: x86_64")
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
  ELSE(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")
    ADD_DEFINITIONS(-DARCH_UNKNOWN)
    message("-- Architecture: unknown")
  ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")

ELSE(USE_ASM AND (NOT YASM_PATH MATCHES "^.*-NOTFOUND"))
    message("-- Assembler optimizations are disabled")
ENDIF(USE_ASM AND (NOT YASM_PATH MATCHES "^.*-NOTFOUND"))

SET(libffap_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libffap_RCC_SRCS ${libffap_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(ffap MODULE ${libffap_SRCS} ${libffap_RCC_SRCS} ${libffap_HDRS} ${YASM_OBJS})
target_link_libraries(ffap Qt5::Widgets -lqmmp ${ffap_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS ffap DESTINATION ${LIB_DIR}/qmmp/Input)
