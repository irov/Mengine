/* -*- C -*- ***********************************************
Copyright (c) 2000, BeOpen.com.
Copyright (c) 1995-2000, Corporation for National Research Initiatives.
Copyright (c) 1990-1995, Stichting Mathematisch Centrum.
All rights reserved.

See the file "Misc/COPYRIGHT" for information on usage and
redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
******************************************************************/

/* Module configuration */

/* !!! !!! !!! This file is edited by the makesetup script !!! !!! !!! */

/* This file contains the table of built-in modules.
   See init_builtin() in import.c. */

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

@PY_STATIC_MODULES_INIT_FUNCTIONS@
/* -- ADDMODULE MARKER 1 -- */
extern void initzipimport(void);
extern void initzlib(void); 
extern void PyMarshal_Init(void);
extern void initimp(void);
extern void initgc(void);
extern void init_ast(void);
extern void initunicodedata(void);
//extern void initnt(void);
extern void initcPickle(void);
extern void initcStringIO(void);

struct _inittab _PyImport_Inittab[] = {
@PY_STATIC_MODULES_INITTAB@

/* -- ADDMODULE MARKER 2 -- */

    //{"nt", initnt}, /* Use the NT os functions, not posix */

	/* This module lives in marshal.c */
	{"marshal", PyMarshal_Init},

	/* This lives in import.c */
	{"imp", initimp},

	/* This lives in Python/Python-ast.c */
	{"_ast", init_ast},
    {"zipimport", initzipimport},
    {"zlib", initzlib}, 
	/* These entries are here for sys.builtin_module_names */
	{"__main__", NULL},
	{"__builtin__", NULL},
	{"sys", NULL},
	{"exceptions", NULL},

	/* This lives in gcmodule.c */
	{"gc", initgc},
    //{"unicodedata", initunicodedata},
    
    {"cPickle", initcPickle},
    {"cStringIO", initcStringIO},
    
	/* Sentinel */
	{0, 0}
};


#ifdef __cplusplus
}
#endif

