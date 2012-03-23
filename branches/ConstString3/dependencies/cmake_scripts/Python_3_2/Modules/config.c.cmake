

//extern void PyMarshal_Init(void);
//extern void initimp(void);
//extern void initgc(void);
//extern void init_ast(void);
//extern void initnt(void);
#include "Python.h"
extern PyObject* PyInit_gc(void);
extern PyObject* PyMarshal_Init(void);
extern PyObject* PyInit_imp(void);
extern PyObject* PyInit__ast(void);



struct _inittab _PyImport_Inittab[] = {


/* -- ADDMODULE MARKER 2 -- */

    //{"nt", PyInit_nt}, /* Use the NT os functions, not posix */

	/* This module lives in marshal.c */
	{"marshal", PyMarshal_Init},

	/* This lives in import.c */
	{"imp", PyInit_imp},

	/* This lives in Python/Python-ast.c */
	{"_ast", PyInit__ast},

	/* These entries are here for sys.builtin_module_names */
	{"__main__", NULL},
	{"__builtin__", NULL},
	{"sys", NULL},
	{"exceptions", NULL},

	/* This lives in gcmodule.c */
	{"gc", PyInit_gc},
    
	/* Sentinel */
	{0, 0}
};

