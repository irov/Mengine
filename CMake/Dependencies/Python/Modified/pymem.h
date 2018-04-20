#ifndef Py_PYMEM_H
#define Py_PYMEM_H

#include "pyport.h"
#include "stdex/allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

//#	define stdex_malloc malloc
//#	define stdex_realloc realloc
//#	define stdex_free free

PyAPI_FUNC(void *) PyMem_Malloc(size_t);
PyAPI_FUNC(void *) PyMem_Realloc(void *, size_t);
PyAPI_FUNC(void) PyMem_Free(void *);

#define PyMem_MALLOC(n)		((size_t)(n) > (size_t)PY_SSIZE_T_MAX ? NULL \
    : stdex_malloc((n) ? (n) : 1, "Python"))
#define PyMem_REALLOC(p, n)	((size_t)(n) > (size_t)PY_SSIZE_T_MAX  ? NULL \
    : stdex_realloc((p), (n) ? (n) : 1, "Python"))

#define PyMem_FREE(p)		stdex_free(p, "Python")


#define PyMem_New(type, n) \
  ( ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	( (type *) PyMem_Malloc((n) * sizeof(type)) ) )
#define PyMem_NEW(type, n) \
  ( ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	( (type *) PyMem_MALLOC((n) * sizeof(type)) ) )

/*
 * The value of (p) is always clobbered by this macro regardless of success.
 * The caller MUST check if (p) is NULL afterwards and deal with the memory
 * error if so.  This means the original value of (p) MUST be saved for the
 * caller's memory error handler to not lose track of it.
 */
#define PyMem_Resize(p, type, n) \
  ( (p) = ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	(type *) PyMem_Realloc((p), (n) * sizeof(type)) )
#define PyMem_RESIZE(p, type, n) \
  ( (p) = ((size_t)(n) > PY_SSIZE_T_MAX / sizeof(type)) ? NULL :	\
	(type *) PyMem_REALLOC((p), (n) * sizeof(type)) )

/* PyMem{Del,DEL} are left over from ancient days, and shouldn't be used
 * anymore.  They're just confusing aliases for PyMem_{Free,FREE} now.
 */
#define PyMem_Del		PyMem_Free
#define PyMem_DEL		PyMem_FREE

#ifdef __cplusplus
}
#endif

#endif /* !Py_PYMEM_H */
