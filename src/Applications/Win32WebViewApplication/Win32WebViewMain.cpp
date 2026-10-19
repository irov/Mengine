#include "Win32WebViewApplication.h"

//////////////////////////////////////////////////////////////////////////
int WINAPI wWinMain( HINSTANCE _instance, HINSTANCE _previous, PWSTR _commandLine, int _show )
{
    MENGINE_UNUSED( _previous );
    MENGINE_UNUSED( _commandLine );
    MENGINE_UNUSED( _show );

    HRESULT result = ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );

    if( FAILED( result ) == true )
    {
        return 1;
    }

    const WCHAR * command = ::GetCommandLineW();
    int count = 0;
    WCHAR ** arguments = ::CommandLineToArgvW( command, &count );

    if( arguments == nullptr )
    {
        ::CoUninitialize();

        return 1;
    }

    Mengine::Win32WebViewApplication application;

    bool initialized = application.initialize( _instance, count, arguments );
    ::LocalFree( arguments );

    if( initialized == false )
    {
        application.finalize();
        ::CoUninitialize();

        return 1;
    }

    int code = application.loop();

    application.finalize();
    ::CoUninitialize();

    return code;
}
//////////////////////////////////////////////////////////////////////////
