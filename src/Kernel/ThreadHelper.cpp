#include "ThreadHelper.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"

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
        void dispatchMainThreadEvent( const LambdaDispatchMainThreadEvent & _event )
        {
            if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
            {
                _event();

                return;
            }

            THREAD_SERVICE()
                ->dispatchMainThreadEvent( _event );
        }
        //////////////////////////////////////////////////////////////////////////
        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const LambdaThreadRunner & _lambda, uint32_t _sleep, const DocumentInterfacePtr & _doc )
        {
            if( SERVICE_IS_INITIALIZE( ThreadSystemInterface ) == false )
            {
                return nullptr;
            }

            ThreadIdentityInterfacePtr identity = THREAD_SYSTEM()
                ->createThreadIdentity( _description, _priority, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread identity" );

            identity->run( _lambda, _sleep, _doc );

            return identity;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}