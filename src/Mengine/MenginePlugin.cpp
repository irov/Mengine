#include "MenginePlugin.h"

#include "Interface/ServiceInterface.h"
#include "Interface/BootstrapperInterface.h"

#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( Bootstrapper );
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DLL)
extern "C"
{
#endif
    //////////////////////////////////////////////////////////////////////////
    Mengine::ServiceProviderInterface * initializeMengine()
    {
        Mengine::ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return nullptr;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        if( SERVICE_CREATE( AllocatorSystem, nullptr ) == false )
        {
            return nullptr;
        }

        if( SERVICE_CREATE( StringizeService, nullptr ) == false )
        {
            return nullptr;
        }

        if( SERVICE_CREATE( DocumentService, nullptr ) == false )
        {
            return nullptr;
        }

        return serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool bootstrapMengine()
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
    void finalizeMengine()
    {
        SERVICE_FINALIZE( Bootstrapper );
        SERVICE_DESTROY( Bootstrapper );

        SERVICE_FINALIZE( DocumentService );
        SERVICE_DESTROY( DocumentService );

        SERVICE_FINALIZE( StringizeService );
        SERVICE_DESTROY( StringizeService );

        SERVICE_FINALIZE( AllocatorSystem );
        SERVICE_DESTROY( AllocatorSystem );

        Mengine::ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_PUT();
        SERVICE_PROVIDER_FINALIZE( serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DLL)
}
#endif
//////////////////////////////////////////////////////////////////////////