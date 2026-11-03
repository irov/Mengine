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
            ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

            if( serviceProvider == nullptr )
            {
                _event();

                return;
            }

            if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
            {
                _event();

                return;
            }

            static const ServiceInterfacePtr & service = serviceProvider->getService( SERVICE_ID( ThreadServiceInterface ) );

            ThreadServiceInterface * thread = static_cast<ThreadServiceInterface *>(service.get());

            thread->dispatchMainThreadEvent( _event );
        }
        //////////////////////////////////////////////////////////////////////////
        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc )
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