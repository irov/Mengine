/* Module configuration */

/* This file contains the table of built-in modules.
   See init_builtin() in import.c. */

#include "Python.h"

extern PyObject* PyInit_array(void);
extern PyObject* PyInit_binascii(void);
extern PyObject* PyInit_cmath(void);
extern PyObject* PyInit_errno(void);
extern PyObject* PyInit_gc(void);
extern PyObject* PyInit_math(void);
extern PyObject* PyInit_operator(void);
extern PyObject* PyInit_signal(void);
#ifdef WIN32
extern PyObject* PyInit__locale(void);
#endif
extern PyObject* PyInit__codecs(void);
extern PyObject* PyInit__weakref(void);
extern PyObject* PyInit_xxsubtype(void);
extern PyObject* PyInit_itertools(void);
extern PyObject* PyInit__collections(void);
extern PyObject* PyInit__heapq(void);
extern PyObject* PyInit__bisect(void);
extern PyObject* PyInit__symtable(void);
extern PyObject* PyInit__sre(void);
extern PyObject* PyInit__struct(void);
extern PyObject* PyInit__functools(void);

extern PyObject* PyInit__subprocess(void);
extern PyObject* PyInit__pickle(void);
extern PyObject* _PyWarnings_Init(void);
extern PyObject* PyInit__string(void);

/* tools/freeze/makeconfig.py marker for additional "extern" */
/* -- ADDMODULE MARKER 1 -- */

extern PyObject* PyMarshal_Init(void);
extern PyObject* PyInit_imp(void);

struct _inittab _PyImport_Inittab[] = {

    {"array", PyInit_array},
    {"binascii", PyInit_binascii},
    {"cmath", PyInit_cmath},
    {"errno", PyInit_errno},
    {"gc", PyInit_gc},
    {"math", PyInit_math},
    {"operator", PyInit_operator},
    {"signal", PyInit_signal},
    {"_weakref", PyInit__weakref},
    {"_bisect", PyInit__bisect},
    {"itertools", PyInit_itertools},
    {"_collections", PyInit__collections},
    {"_symtable", PyInit__symtable},
    {"_struct", PyInit__struct},
    {"_functools", PyInit__functools},

/* tools/freeze/makeconfig.py marker for additional "_inittab" entries */
/* -- ADDMODULE MARKER 2 -- */
   

    /* These entries are here for sys.builtin_module_names */
    {"__main__", NULL},
    {"builtins", NULL},
    {"sys", NULL},
    {"_warnings", _PyWarnings_Init},
    {"_string", PyInit__string},

    {"_pickle", PyInit__pickle},

    /* Sentinel */
    {0, 0}
};
