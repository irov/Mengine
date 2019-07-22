#include "NodeDebuggerApp.h"

#include "Config/String.h"

#include "stdex/allocator.h"

#include "Environment/Windows/WindowsIncluder.h"

static Mengine::String WideToUtf8( const Mengine::WString & wideStr )
{
    Mengine::String result;

    const int inSize = static_cast<int>( wideStr.size() );
    const int outSize = ::WideCharToMultiByte( CP_UTF8, 0, wideStr.c_str(), inSize, nullptr, 0, nullptr, nullptr );

    if( outSize > 0 )
    {
        result.resize( outSize + 1 );
        ::WideCharToMultiByte( CP_UTF8, 0, wideStr.c_str(), inSize, result.data(), outSize, nullptr, nullptr );
    }

    return result;
}

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    stdex_allocator_initialize();

    {
        Mengine::String address;
        uint16_t port = 0;

        Mengine::WString cmdLine( lpCmdLine );
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

    stdex_allocator_finalize();

    return 0;
}
