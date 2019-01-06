#include "NodeDebuggerApp.h"

#include "stdex/allocator.h"

static std::string WideToUtf8( const std::wstring& wideStr )
{
    std::string result;

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
    (void)lpCmdLine;
    (void)nShowCmd;

    stdex_allocator_initialize();

    {
        std::string address;
        uint16_t port = 0;

        std::wstring cmdLine( lpCmdLine );
        if( !cmdLine.empty() )
        {
            std::wstring::size_type columnPos = cmdLine.find( TEXT( ':' ) );
            if( std::wstring::npos != columnPos )
            {
                std::wstring wideAddress = cmdLine.substr( 0, columnPos );
                std::wstring widePort = cmdLine.substr( columnPos + 1 );

                port = static_cast<uint16_t>( std::stoi( widePort ) & 0xFFFF );
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
