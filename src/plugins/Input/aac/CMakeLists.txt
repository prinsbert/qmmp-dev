project(libaac)

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

# libfaad
check_include_file_cxx(neaacdec.h FAAD_FOUND)

if(FAAD_FOUND)
    set(CMAKE_REQUIRED_LIBRARIES faad)
check_cxx_source_compiles("
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <neaacdec.h>

int main(int argc, char **argv)
{
#ifdef OLD_FAAD_PROTO
unsigned long int srate;
#else
uint32_t srate;
#endif
uint32_t l;
uint8_t *d;
unsigned char chan;
void    *_instance;
        NeAACDecInit2(_instance, d,l, &srate,&chan);
        return 0;
}" FAAD_MODIFIED)
    if(FAAD_MODIFIED)
        add_definitions(-DFAAD_MODIFIED)
    endif(FAAD_MODIFIED)
endif(FAAD_FOUND)

set(libaac_SRCS
  decoder_aac.cpp
  decoderaacfactory.cpp
  aacfile.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(FAAD_FOUND)
    add_library(aac MODULE ${libaac_SRCS} ${libaac_RCC_SRCS})
    target_link_libraries(aac PRIVATE Qt5::Widgets libqmmp -lfaad PkgConfig::TAGLIB)
    install(TARGETS aac DESTINATION ${PLUGIN_DIR}/Input)
endif(FAAD_FOUND)
