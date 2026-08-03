#include "NodeDebuggerApp.h"

#include "Interface/DocumentServiceInterface.h"
#include "Interface/ServiceProviderInterface.h"

#include "ToolUtils/ToolUtils.h"

#include <cstdlib>
#include <string>

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
//////////////////////////////////////////////////////////////////////////
static int runNodeDebugger()
{
    Mengine::ServiceProviderInterface * serviceProvider = nullptr;

    if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
    {
        return EXIT_FAILURE;
    }

    SERVICE_PROVIDER_SETUP( serviceProvider );

    SERVICE_CREATE( AllocatorSystem, nullptr );
    SERVICE_CREATE( EnumeratorService, nullptr );
    SERVICE_CREATE( StringizeService, nullptr );
    SERVICE_CREATE( DocumentService, nullptr );

    Mengine::String address;
    uint16_t port = 0;

    const std::vector<std::wstring> & arguments = get_command_line_arguments();

    for( size_t index = 1; index != arguments.size(); ++index )
    {
        const std::wstring & argument = arguments[index];

        if( argument.rfind( L"ip=", 0 ) != 0 )
        {
            continue;
        }

        const size_t colon = argument.find( L':', 3 );
        const std::wstring wideAddress = argument.substr( 3, colon == std::wstring::npos ? std::wstring::npos : colon - 3 );
        address = unicode_to_utf8( wideAddress );

        if( colon != std::wstring::npos )
        {
            port = static_cast<uint16_t>( std::wcstoul( argument.c_str() + colon + 1, nullptr, 10 ) & 0xFFFF );
        }
    }

    Mengine::NodeDebuggerApp app;

    if( app.Initialize( address, port ) == true )
    {
        app.Loop();
        app.Shutdown();
    }

    SERVICE_FINALIZE( DocumentService );
    SERVICE_FINALIZE( StringizeService );
    SERVICE_FINALIZE( EnumeratorService );
    SERVICE_FINALIZE( AllocatorSystem );

    SERVICE_DESTROY( DocumentService );
    SERVICE_DESTROY( StringizeService );
    SERVICE_DESTROY( EnumeratorService );
    SERVICE_DESTROY( AllocatorSystem );

    SERVICE_PROVIDER_FINALIZE( serviceProvider );

    return EXIT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE _instance, HINSTANCE _previousInstance, LPSTR _commandLine, int _showCommand )
{
    (void)_instance;
    (void)_previousInstance;
    (void)_commandLine;
    (void)_showCommand;

    return runNodeDebugger();
}
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    return runNodeDebugger();
}
//////////////////////////////////////////////////////////////////////////
#endif
