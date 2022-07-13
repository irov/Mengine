#include "NodeDebuggerApp.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/AllocatorServiceInterface.h"
#include "Interface/DocumentServiceInterface.h"

#include "Kernel/String.h"

#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( DocumentService );
//////////////////////////////////////////////////////////////////////////
static Mengine::String WideToUtf8( const Mengine::WString & wideStr )
{
    Mengine::String result;

    int inSize = static_cast<int>(wideStr.size());
    int outSize = ::WideCharToMultiByte( CP_UTF8, 0, wideStr.c_str(), inSize, nullptr, 0, nullptr, nullptr );

    if( outSize > 0 )
    {
        result.resize( outSize + 1 );
        ::WideCharToMultiByte( CP_UTF8, 0, wideStr.c_str(), inSize, result.data(), outSize, nullptr, nullptr );
    }

    return result;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    PWSTR pwCmdLine = GetCommandLineW();

    {
        Mengine::ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorService, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        {
            Mengine::String address;
            uint16_t port = 0;

            Mengine::WString cmdLine( pwCmdLine );
            if( !cmdLine.empty() )
            {
                Mengine::WString::size_type columnPos = cmdLine.find( TEXT( ':' ) );
                if( Mengine::WString::npos != columnPos )
                {
                    Mengine::WString wideAddress = cmdLine.substr( 0, columnPos );
                    Mengine::WString widePort = cmdLine.substr( columnPos + 1 );

                    Mengine::WChar * ptr;
                    port = static_cast<uint16_t>(::wcstol( widePort.c_str(), &ptr, 10 ) & 0xFFFF);
                    address = WideToUtf8( wideAddress );
                }
            }

            Mengine::NodeDebuggerApp app;

            if( app.Initialize( address, port ) )
            {
                app.Loop();
                app.Shutdown();
            }
        }

        SERVICE_FINALIZE( DocumentService );
        SERVICE_DESTROY( DocumentService );

        SERVICE_FINALIZE( StringizeService );
        SERVICE_DESTROY( StringizeService );

        SERVICE_FINALIZE( AllocatorService );
        SERVICE_DESTROY( AllocatorService );

        SERVICE_PROVIDER_FINALIZE( serviceProvider );
    }

    return 0;
}
