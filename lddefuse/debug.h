#ifndef _DEBUG_H
#define _DEBUG_H

#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string>
#include <sstream>

static inline uint64_t debuggettime()
{
    /*
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    */
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    uint64_t ns = tp.tv_sec * 1000000000 + tp.tv_nsec;
    return ns;
}

#ifndef REFTHREAD
    #define REFTHREAD ""
#endif

#ifndef DEBUG_HEADER
    #define DEBUG_HEADER \
        debuggettime() << " " << getpid() << " " << pthread_self() << " " << REFTHREAD
#endif

#define DEBUG_PRINT(value) \
    { \
        std::stringstream DEBUG_OUT_STREAM; \
        DEBUG_OUT_STREAM << DEBUG_HEADER << " " << __PRETTY_FUNCTION__ << " : " << value << std::endl; \
        std::string DEBUG_OUT_STRING = DEBUG_OUT_STREAM.str(); \
        syscall(SYS_write, STDERR_FILENO, DEBUG_OUT_STRING.data(), DEBUG_OUT_STRING.length()); \
    }

#define DEBUG_ENTER \
    { \
        std::stringstream DEBUG_OUT_STREAM; \
        DEBUG_OUT_STREAM << DEBUG_HEADER << " ENT " << __PRETTY_FUNCTION__ << std::endl; \
        std::string DEBUG_OUT_STRING = DEBUG_OUT_STREAM.str(); \
        syscall(SYS_write, STDERR_FILENO, DEBUG_OUT_STRING.data(), DEBUG_OUT_STRING.length()); \
    }

#define DEBUG_EXIT(value) \
    { \
        std::stringstream DEBUG_OUT_STREAM; \
        DEBUG_OUT_STREAM << DEBUG_HEADER << " EXT " << __PRETTY_FUNCTION__ << " : " << value << std::endl; \
        std::string DEBUG_OUT_STRING = DEBUG_OUT_STREAM.str(); \
        syscall(SYS_write, STDERR_FILENO, DEBUG_OUT_STRING.data(), DEBUG_OUT_STRING.length()); \
    }

/*
#define DEBUG_PRINT(value)
#define DEBUG_ENTER
#define DEBUG_EXIT(value) \
    { \
	    if(value == EINTR) {\
        std::stringstream DEBUG_OUT_STREAM; \
        DEBUG_OUT_STREAM << DEBUG_HEADER << " EXT " << __PRETTY_FUNCTION__ << " : " << value << std::endl; \
        std::string DEBUG_OUT_STRING = DEBUG_OUT_STREAM.str(); \
        syscall(SYS_write, STDERR_FILENO, DEBUG_OUT_STRING.data(), DEBUG_OUT_STRING.length()); \
		} \
    }
*/

#define DEBUG_PRINT_BUFFER(BPTR, BSIZE) \
    { \
        char DEBUG_PRINT_BUFFER[128]; \
        char* DPTR = DEBUG_PRINT_BUFFER; \
        for(int i = 0; i < BSIZE; i++) { \
            if(i > 0 && i % 16 == 0) { \
                DEBUG_PRINT(DEBUG_PRINT_BUFFER); \
                DPTR = DEBUG_PRINT_BUFFER; \
            } \
            if(i % 4 == 0) { \
                DPTR += sprintf(DPTR, " "); \
            } \
            DPTR += sprintf(DPTR, "%02X", ((unsigned char*)BPTR)[i]); \
        } \
        DEBUG_PRINT(DEBUG_PRINT_BUFFER); \
    }
    
#endif // _DEBUG_H
