#ifdef __cplusplus
extern "C" {
#endif

#   include "Python.h"

#if defined _MSC_VER
    int _PyVerify_fd(int fd)
    {
        (void)fd;

        return 1;
    }
#   endif

#define IS_LITTLE_ENDIAN (int)*(unsigned char*)&one
    PyObject *
        PyLong_FromSsize_t(Py_ssize_t ival)
    {
        Py_ssize_t bytes = ival;
        int one = 1;
        return _PyLong_FromByteArray((unsigned char *)&bytes,
            SIZEOF_SIZE_T, IS_LITTLE_ENDIAN, 1);
    }

    PyObject *
        _PyLong_FromSize_t(size_t ival)
    {
        return PyLong_FromSsize_t(ival);
    }
    
#undef IS_LITTLE_ENDIAN

#ifdef __cplusplus
}
#endif
