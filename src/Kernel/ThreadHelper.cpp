#include "ThreadHelper.h"

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/ThreadServiceInterface.h"


#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadJob.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const ConstString & getCurrentThreadName()
        {
            if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
            {
                return ConstString::none();
            }
             
            const ConstString & threadName = THREAD_SERVICE()
                ->getCurrentThreadName();

            return threadName;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}