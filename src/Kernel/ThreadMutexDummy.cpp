#include "ThreadMutexDummy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void ThreadMutexDummy::lock()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadMutexDummy::unlock()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadMutexDummy::tryLock()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}