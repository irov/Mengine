/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: Define a consistent set of types on each platform.

 ********************************************************************/
#ifndef _OS_TYPES_H
#define _OS_TYPES_H


//////////////////////////////////////////////////////////////////////////
typedef void * (*OGG_malloc_t)(size_t _size, void * _ud);
typedef void * (*OGG_ñalloc_t)(size_t _num, size_t _size, void * _ud);
typedef void * (*OGG_realloc_t)(void * _ptr, size_t _size, void * _ud);
typedef void (*OGG_free_t)(void * _ptr, void * _ud);
//////////////////////////////////////////////////////////////////////////    
static OGG_malloc_t s_OGG_malloc;
static OGG_ñalloc_t s_OGG_calloc;
static OGG_realloc_t s_OGG_realloc;
static OGG_free_t s_OGG_free;
static void * s_OGG_ud;
//////////////////////////////////////////////////////////////////////////
inline static void set_OGG_allocator( OGG_malloc_t _malloc, OGG_ñalloc_t _calloc, OGG_realloc_t _realloc, OGG_free_t _free, void * _ud )
{
    s_OGG_malloc = _malloc;
    s_OGG_calloc = _calloc;
    s_OGG_realloc = _realloc;
    s_OGG_free = _free;
    s_OGG_ud = _ud;
}
//////////////////////////////////////////////////////////////////////////
/* make it easy on the folks that want to compile the libs with a
   different malloc than stdlib */
#define _ogg_malloc(n)     ((*s_OGG_malloc)(n, s_OGG_ud))
#define _ogg_calloc(c,n)   ((*s_OGG_calloc)(c, n, s_OGG_ud))
#define _ogg_realloc(p, n) ((*s_OGG_realloc)(p, n, s_OGG_ud))
#define _ogg_free(p)       ((*s_OGG_free)(p, s_OGG_ud))


#if defined(_WIN32)

#  if defined(__CYGWIN__)
#    include <stdint.h>
     typedef int16_t ogg_int16_t;
     typedef uint16_t ogg_uint16_t;
     typedef int32_t ogg_int32_t;
     typedef uint32_t ogg_uint32_t;
     typedef int64_t ogg_int64_t;
     typedef uint64_t ogg_uint64_t;
#  elif defined(__MINGW32__)
#    include <sys/types.h>
     typedef short ogg_int16_t;
     typedef unsigned short ogg_uint16_t;
     typedef int ogg_int32_t;
     typedef unsigned int ogg_uint32_t;
     typedef long long ogg_int64_t;
     typedef unsigned long long ogg_uint64_t;
#  elif defined(__MWERKS__)
     typedef long long ogg_int64_t;
     typedef unsigned long long ogg_uint64_t;
     typedef int ogg_int32_t;
     typedef unsigned int ogg_uint32_t;
     typedef short ogg_int16_t;
     typedef unsigned short ogg_uint16_t;
#  else
#    if defined(_MSC_VER) && (_MSC_VER >= 1800) /* MSVC 2013 and newer */
#      include <stdint.h>
       typedef int16_t ogg_int16_t;
       typedef uint16_t ogg_uint16_t;
       typedef int32_t ogg_int32_t;
       typedef uint32_t ogg_uint32_t;
       typedef int64_t ogg_int64_t;
       typedef uint64_t ogg_uint64_t;
#    else
       /* MSVC/Borland */
       typedef __int64 ogg_int64_t;
       typedef __int32 ogg_int32_t;
       typedef unsigned __int32 ogg_uint32_t;
       typedef unsigned __int64 ogg_uint64_t;
       typedef __int16 ogg_int16_t;
       typedef unsigned __int16 ogg_uint16_t;
#    endif
#  endif

#elif (defined(__APPLE__) && defined(__MACH__)) /* MacOS X Framework build */

#  include <sys/types.h>
   typedef int16_t ogg_int16_t;
   typedef uint16_t ogg_uint16_t;
   typedef int32_t ogg_int32_t;
   typedef uint32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;
   typedef uint64_t ogg_uint64_t;

#elif defined(__HAIKU__)

  /* Haiku */
#  include <sys/types.h>
   typedef short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;
   typedef unsigned long long ogg_uint64_t;

#elif defined(__BEOS__)

   /* Be */
#  include <inttypes.h>
   typedef int16_t ogg_int16_t;
   typedef uint16_t ogg_uint16_t;
   typedef int32_t ogg_int32_t;
   typedef uint32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;
   typedef uint64_t ogg_uint64_t;

#elif defined (__EMX__)

   /* OS/2 GCC */
   typedef short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;
   typedef unsigned long long ogg_uint64_t;


#elif defined (DJGPP)

   /* DJGPP */
   typedef short ogg_int16_t;
   typedef int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long ogg_int64_t;
   typedef unsigned long long ogg_uint64_t;

#elif defined(R5900)

   /* PS2 EE */
   typedef long ogg_int64_t;
   typedef unsigned long ogg_uint64_t;
   typedef int ogg_int32_t;
   typedef unsigned ogg_uint32_t;
   typedef short ogg_int16_t;

#elif defined(__SYMBIAN32__)

   /* Symbian GCC */
   typedef signed short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef signed int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long int ogg_int64_t;
   typedef unsigned long long int ogg_uint64_t;

#elif defined(__TMS320C6X__)

   /* TI C64x compiler */
   typedef signed short ogg_int16_t;
   typedef unsigned short ogg_uint16_t;
   typedef signed int ogg_int32_t;
   typedef unsigned int ogg_uint32_t;
   typedef long long int ogg_int64_t;
   typedef unsigned long long int ogg_uint64_t;

#elif defined(__ANDROID__)

#  include <stdint.h>
   typedef int16_t ogg_int16_t;
   typedef uint16_t ogg_uint16_t;
   typedef int32_t ogg_int32_t;
   typedef uint32_t ogg_uint32_t;
   typedef int64_t ogg_int64_t;
   typedef uint64_t ogg_uint64_t;

#elif defined(__linux__)

#  include <stdint.h>
typedef int16_t ogg_int16_t;
typedef uint16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef uint32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;
typedef uint64_t ogg_uint64_t;

#else

#  include <ogg/config_types.h>

#endif

#endif  /* _OS_TYPES_H */
