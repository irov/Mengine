#ifndef Py_PYMEM_H
#define Py_PYMEM_H

#include "pyport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void * ctx;

    /* allocate a memory block */
    void * (*malloc) (void * ctx, size_t size);

    /* allocate a memory block initialized by zeros */
    void * (*calloc) (void * ctx, size_t nelem, size_t elsize);

    /* allocate or resize a memory block */
    void * (*realloc) (void * ctx, void * ptr, size_t new_size);

    /* release a memory block */
    void (*free) (void * ctx, void * ptr);
} PyMemAllocatorEx;

PyAPI_FUNC(void) PyMem_SetAllocator( PyMemAllocatorEx * allocator );
PyAPI_FUNC(PyMemAllocatorEx *) PyMem_GetAllocator();

PyAPI_FUNC(void *) PyMem_Malloc(size_t);
PyAPI_FUNC(void *) PyMem_Realloc(void *, size_t);
PyAPI_FUNC(void) PyMem_Free(void *);

#define PyMem_MALLOC(n)		((size_t)(n) > (size_t)PY_SSIZE_T_MAX ? NULL \
    : PyMem_GetAllocator()->malloc(PyMem_GetAllocator()->ctx, (n) ? (n) : 1))
#define PyMem_REALLOC(p, n)	((size_t)(n) > (size_t)PY_SSIZE_T_MAX  ? NULL \
    : PyMem_GetAllocator()->realloc(PyMem_GetAllocator()->ctx, (p), (n) ? (n) : 1))

#define PyMem_FREE(p)		PyMem_GetAllocator()->free(PyMem_GetAllocator()->ctx, p)


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
