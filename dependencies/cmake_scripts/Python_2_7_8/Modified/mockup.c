#   include "Python.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#if defined _MSC_VER && _MSC_VER >= 1200
    int _PyVerify_fd(int fd)
    {
        (void)fd;

        return 1;
    }
#endif

#   define IS_LITTLE_ENDIAN (int)*(unsigned char*)&one

    PyObject *
        PyLong_FromSsize_t(Py_ssize_t ival)
    {
        Py_ssize_t bytes = ival;
        int one = 1;
        return _PyLong_FromByteArray((unsigned char *)&bytes,
            SIZEOF_SIZE_T, IS_LITTLE_ENDIAN, 1);
    }

#   undef IS_LITTLE_ENDIAN

    PyObject *
        PyLong_FromSize_t(size_t ival)
    {
        PyObject * obj = PyInt_FromLong(ival);

        return obj;
    }

#   ifdef __cplusplus
}
#   endif
