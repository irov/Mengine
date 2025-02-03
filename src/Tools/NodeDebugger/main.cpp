#include "NodeDebuggerApp.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/DocumentServiceInterface.h"

#include "Kernel/String.h"

#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
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

    {
        Mengine::ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorSystem, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        {
            PWSTR pwCmdLine = ::GetCommandLineW();

            int32_t pNumArgs;
            LPWSTR * szArglist = ::CommandLineToArgvW( pwCmdLine, &pNumArgs );

            Mengine::String address;
            uint16_t port = 0;

            for( int32_t i = 1; i != pNumArgs; ++i )
            {
                LPWSTR arg = szArglist[i];

                LPWSTR arg_ip = Mengine::StdString::wcsstr( arg, L"ip=" );

                if( arg_ip != nullptr )
                {
                    LPWSTR arg_port = Mengine::StdString::wcsstr( arg, L":" );

                    if( arg_port != nullptr )
                    {
                        address.assign( arg + Mengine::StdString::wcslen( L"ip=" ), arg_port );

                        LPWSTR ptr;
                        port = Mengine::StdString::wcstol( arg_port + Mengine::StdString::wcslen( L":" ), &ptr, 10 ) & 0xFFFF;
                    }
                    else
                    {
                        address.assign( arg + Mengine::StdString::wcslen( L"ip=" ), arg_ip );
                    }
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
        SERVICE_FINALIZE( StringizeService );
        SERVICE_FINALIZE( AllocatorSystem );

        SERVICE_DESTROY( DocumentService );
        SERVICE_DESTROY( StringizeService );
        SERVICE_DESTROY( AllocatorSystem );

        SERVICE_PROVIDER_FINALIZE( serviceProvider );
    }

    return 0;
}
