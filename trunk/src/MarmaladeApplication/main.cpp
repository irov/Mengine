#   include "Config/Config.h"

#   include "MarmaladeApplication.h"

#   include "s3eMemory.h"

s3eMemoryUsrMgr _oldCallback;
s3eMemoryUsrMgr _callback;

static void _s3eFreeFn(void *item)
{
    _oldCallback.m_Free(item);
}

static void * _s3eMallocFn(int size) 
{
    void * p = _oldCallback.m_Malloc(size);

    return p;
}

static void * _s3eReallocFn(void *item, int size) 
{
    void * p = _oldCallback.m_Realloc(item, size);

    return p;
}

int main()
{
    _callback.m_Free = &_s3eFreeFn;
    _callback.m_Malloc = &_s3eMallocFn;
    _callback.m_Realloc = &_s3eReallocFn;
    s3eMemoryGetUserMemMgr(&_oldCallback);
    s3eMemorySetUserMemMgr(&_callback);

    Menge::MarmaladeApplication marApplication;

    if( marApplication.initialize() == false )
    {
        return 0;
    }

    marApplication.loop();

    marApplication.finalize();

    return 0;
}
