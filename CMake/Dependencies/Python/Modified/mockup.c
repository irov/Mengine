#   include "Python.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#if defined _MSC_VER && _MSC_VER >= 1200
#ifdef NDEBUG
    int _PyVerify_fd(int fd)
    {
        (void)fd;

        return 1;
    }
#endif
#endif

#   ifdef __cplusplus
}
#   endif
