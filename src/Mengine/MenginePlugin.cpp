#include "MenginePlugin.h"

#include "Interface/ServiceInterface.h"
#include "Interface/BootstrapperInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/DynamicCast.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( Bootstrapper );
//////////////////////////////////////////////////////////////////////////
extern "C" Mengine::ServiceProviderInterface * API_MengineCreate()
{
    if( SERVICE_PROVIDER_EXIST() == true )
    {
        return nullptr;
    }

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

    if( SERVICE_CREATE( TimeSystem, nullptr ) == false )
    {
        return nullptr;
    }

    if( SERVICE_CREATE( EnumeratorService, nullptr ) == false )
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
extern "C" bool API_MengineBootstrap()
{
    if( SERVICE_CREATE( Bootstrapper, MENGINE_DOCUMENT_FUNCTION ) == false )
    {
        return false;
    }

    if( BOOTSTRAPPER_SERVICE()
        ->initialize() == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" bool API_MengineRun()
{
    if( BOOTSTRAPPER_SERVICE()
        ->run() == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" void API_MengineFinalize()
{
    SERVICE_FINALIZE( Bootstrapper );
    SERVICE_DESTROY( Bootstrapper );

    SERVICE_FINALIZE( DocumentService );
    SERVICE_DESTROY( DocumentService );

    SERVICE_FINALIZE( StringizeService );
    SERVICE_DESTROY( StringizeService );

    SERVICE_FINALIZE( EnumeratorService );
    SERVICE_DESTROY( EnumeratorService );

    SERVICE_FINALIZE( TimeSystem );
    SERVICE_DESTROY( TimeSystem );

    SERVICE_FINALIZE( AllocatorSystem );
    SERVICE_DESTROY( AllocatorSystem );

    Mengine::ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_PUT();
    SERVICE_PROVIDER_FINALIZE( serviceProvider );

    SERVICE_PROVIDER_DESTROY( serviceProvider );
}
//////////////////////////////////////////////////////////////////////////