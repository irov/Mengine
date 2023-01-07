#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern void initimp( void );
    extern void initgc( void );
    extern void initunicodedata( void );
    extern void initmath( void );
    extern void init_sre( void );
    extern void init_struct( void );
    extern void init_weakref( void );
    extern void init_codecs( void );
    extern void init_json( void );
    extern void initbinascii( void );
    extern void initzlib( void );
    extern void PyMarshal_Init( void );
    extern void initcStringIO( void );


#if defined(WIN32) && !defined(NDEBUG) && !defined(__MINGW32__)
    extern void init_lsprof( void );
    extern void initerrno( void );
    extern void init_functools( void );
    extern void inittime( void );
#endif

    struct _inittab _PyImport_Inittab[] = {
        {"__main__", NULL},
        {"__builtin__", NULL},
        {"sys", NULL},
        {"exceptions", NULL},
        {"imp", initimp},
        {"math", initmath},
        {"_sre", init_sre},
        {"_struct", init_struct},
        {"_codecs", init_codecs},
        {"_json", init_json},
        {"binascii", initbinascii},
        {"zlib", initzlib},
        {"gc", initgc},
        {"_weakref", init_weakref},
        {"cStringIO", initcStringIO},
        {"marshal", PyMarshal_Init},

#if defined(WIN32) && !defined(NDEBUG) && !defined(__MINGW32__)
        {"_lsprof", init_lsprof},
        {"errno", initerrno},
        {"_functools", init_functools},
        {"time", inittime},
#endif

        /* Sentinel */
        {0, 0}
    };

#ifdef __cplusplus
}
#endif

