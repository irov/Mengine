#ifndef _CONFIG_TYPES_H
#define _CONFIG_TYPES_H

#   include <dlmalloc.h>

#define _ogg_malloc  dlmalloc
#define _ogg_calloc  dlcalloc
#define _ogg_realloc dlrealloc
#define _ogg_free    dlfree

#ifdef __cplusplus
extern "C" {
#endif

#   ifdef _WIN32
typedef __int64 ogg_int64_t;
typedef __int32 ogg_int32_t;
typedef unsigned __int32 ogg_uint32_t;
typedef __int16 ogg_int16_t;
typedef unsigned __int16 ogg_uint16_t;
#   else
typedef short ogg_int16_t;
typedef unsigned short ogg_uint16_t;
typedef int ogg_int32_t;
typedef unsigned int ogg_uint32_t;
typedef long long ogg_int64_t;
#   endif

#ifdef __cplusplus
}
#endif

#   endif
