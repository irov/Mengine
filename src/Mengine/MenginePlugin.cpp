#include "Interface/ServiceProviderInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/BootstrapperInterface.h"

#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( Bootstrapper );
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    Mengine::ServiceProviderInterface * initializeMengine( void )
    {
        Mengine::ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return nullptr;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        return serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool bootstrapMengine( void )
    {
        if( SERVICE_CREATE( Bootstrapper, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        if( BOOTSTRAPPER_SERVICE()
            ->run() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void finalizeMengine( void )
    {
        SERVICE_FINALIZE( Bootstrapper );
        SERVICE_DESTROY( Bootstrapper );

        SERVICE_FINALIZE( DocumentService );
        SERVICE_DESTROY( DocumentService );

        SERVICE_FINALIZE( AllocatorService );
        SERVICE_DESTROY( AllocatorService );

        Mengine::ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();
        SERVICE_PROVIDER_FINALIZE( serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
}