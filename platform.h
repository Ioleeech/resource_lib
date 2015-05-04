#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef USE_PACKED_ATTRIBUTE
    #define PACKED_STRUCT __attribute__((__packed__))
#else
    #define PACKED_STRUCT
#endif

#endif // __PLATFORM_H__
