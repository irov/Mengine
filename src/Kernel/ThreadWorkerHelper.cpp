#include "ThreadHelper.h"

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/ThreadServiceInterface.h"


#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadJob.h"
#include "Kernel/ThreadWorkerHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool createSimpleThreadWorker( const ConstString & _threadName, EThreadPriority _priority, uint32_t _sleep, const ThreadWorkerInterfacePtr & _worker, const DocumentInterfacePtr & _doc )
        {
            ThreadJobPtr threadJob = THREAD_SERVICE()
                ->createJob( _sleep, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( threadJob, "invalid create thread job" );

            if( THREAD_SERVICE()
                ->createThreadProcessor( _threadName, _priority, _doc ) == false )
            {
                return false;
            }

            if( THREAD_SERVICE()
                ->addTask( _threadName, threadJob, _doc ) == false )
            {
                return false;
            }

            UniqueId workerId = threadJob->addWorker( _worker, _doc );

            if( workerId == INVALID_UNIQUE_ID )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroySimpleThreadWorker( const ConstString & _threadName )
        {
            THREAD_SERVICE()
                ->destroyThreadProcessor( _threadName );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}