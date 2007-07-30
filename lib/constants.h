#ifndef __constants_h
#define __constants_h

#define VERSION "0.1.3"

#if defined(Q_OS_UNIX)
#define LIB_DIR "/lib"
#endif

const unsigned int historySize      = 100;
const unsigned int globalBlockSize  = 2 * 1024;    //2*1024
const unsigned int globalBufferSize = globalBlockSize * 32;
const unsigned int groupOpenTimeout = 750;

#endif // __constants_h
