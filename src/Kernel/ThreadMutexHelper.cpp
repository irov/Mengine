#include "ThreadMutexHelper.h"

#include "Interface/ThreadSystemInterface.h"

namespace Mengine 
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ThreadMutexInterfacePtr createThreadMutex( const DocumentInterfacePtr & _doc )
        {
            ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
                ->createMutex( _doc );

            return mutex;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}