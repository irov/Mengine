#include "ThreadMutexHelper.h"

#include "Interface/ThreadSystemInterface.h"

namespace Mengine 
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ThreadMutexInterfacePtr createThreadMutex( const DocumentInterfacePtr & _doc )
        {
            if( SERVICE_IS_INITIALIZE( ThreadSystemInterface ) == false )
            {
                return nullptr;
            }

            ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
                ->createMutex( _doc );

            return mutex;
        }
        //////////////////////////////////////////////////////////////////////////
        ThreadSharedMutexInterfacePtr createThreadSharedMutex( const DocumentInterfacePtr & _doc )
        {
            if( SERVICE_IS_INITIALIZE( ThreadSystemInterface ) == false )
            {
                return nullptr;
            }

            ThreadSharedMutexInterfacePtr mutex = THREAD_SYSTEM()
                ->createSharedMutex( _doc );

            return mutex;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}