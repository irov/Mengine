#include "Runtime/MengineRuntime.h"

#include "Interface/WebViewBootstrapperInterface.h"

#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( WebViewBootstrapper );
//////////////////////////////////////////////////////////////////////////
extern "C" Mengine::ServiceProviderInterface * API_MengineCreate( const Mengine::Configuration * _configuration )
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

    Mengine::Configuration defaultConfiguration;
    const Mengine::Configuration & configuration = _configuration != nullptr ? *_configuration : defaultConfiguration;
    serviceProvider->setConfiguration( configuration );

    if( SERVICE_CREATE_SAFE( AllocatorSystem, nullptr ) == false )
    {
        API_MengineFinalize();

        return nullptr;
    }

    if( SERVICE_CREATE_SAFE( EnumeratorService, nullptr ) == false )
    {
        API_MengineFinalize();

        return nullptr;
    }

    if( SERVICE_CREATE_SAFE( StringizeService, nullptr ) == false )
    {
        API_MengineFinalize();

        return nullptr;
    }

    if( SERVICE_CREATE_SAFE( DocumentService, nullptr ) == false )
    {
        API_MengineFinalize();

        return nullptr;
    }

    return serviceProvider;
}
//////////////////////////////////////////////////////////////////////////
extern "C" Mengine::ServiceProviderInterface * API_MengineGet()
{
    Mengine::ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_GET();

    return serviceProvider;
}
//////////////////////////////////////////////////////////////////////////
extern "C" bool API_MengineBootstrap()
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return false;
    }

    if( SERVICE_IS_EXIST( Mengine::WebViewBootstrapperInterface ) == true )
    {
        return false;
    }

    if( SERVICE_CREATE_SAFE( WebViewBootstrapper, MENGINE_DOCUMENT_FUNCTION ) == false )
    {
        return false;
    }

    if( WEBVIEW_BOOTSTRAPPER_SERVICE()
        ->initialize() == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" bool API_MengineRun()
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return false;
    }

    if( SERVICE_IS_INITIALIZE( Mengine::WebViewBootstrapperInterface ) == false )
    {
        return false;
    }

    if( WEBVIEW_BOOTSTRAPPER_SERVICE()
        ->run() == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
extern "C" void API_MengineFinalize()
{
    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    SERVICE_FINALIZE( WebViewBootstrapper );
    SERVICE_DESTROY( WebViewBootstrapper );

    SERVICE_FINALIZE( DocumentService );
    SERVICE_DESTROY( DocumentService );

    SERVICE_FINALIZE( StringizeService );
    SERVICE_DESTROY( StringizeService );

    SERVICE_FINALIZE( EnumeratorService );
    SERVICE_DESTROY( EnumeratorService );

    SERVICE_FINALIZE( AllocatorSystem );
    SERVICE_DESTROY( AllocatorSystem );

    Mengine::ServiceProviderInterface * serviceProvider = SERVICE_PROVIDER_PUT();
    SERVICE_PROVIDER_FINALIZE( serviceProvider );
    SERVICE_PROVIDER_DESTROY( serviceProvider );
}
//////////////////////////////////////////////////////////////////////////
