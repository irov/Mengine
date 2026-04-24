#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Configuration.h"

#include "Win32Application.h"

#include "Config/StdString.h"

#if defined(MENGINE_COMPILER_MSVC)
#   include <crtdbg.h>
#endif

//////////////////////////////////////////////////////////////////////////
namespace
{
    //////////////////////////////////////////////////////////////////////////
    // Convert any unhandled SEH exception into a silent process exit instead
    // of letting Windows Error Reporting show a popup.
    //////////////////////////////////////////////////////////////////////////
    static LONG WINAPI Win32SilentUnhandledExceptionFilter( EXCEPTION_POINTERS * /*pExceptionInfo*/ )
    {
        ::TerminateProcess( ::GetCurrentProcess(), (UINT)0xDEADBEEF );

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    // Inlined here (instead of an exported helper) because module-local
    // CRT/SEH/WER state must be configured in EVERY binary that needs it.
    // The Mengine DLL applies its own matching state by reading the same
    // Configuration off ServiceProvider once API_MengineCreate runs;
    // this code configures the EXE side.
    //////////////////////////////////////////////////////////////////////////
    static void Win32ApplyCLIPlatform()
    {
        // Suppress "abort/retry/ignore" critical-error dialog and the
        // "send error report" popup that std::abort() would otherwise raise.
        ::SetErrorMode( SEM_FAILCRITICALERRORS
            | SEM_NOGPFAULTERRORBOX
            | SEM_NOOPENFILEERRORBOX
            | SEM_NOALIGNMENTFAULTEXCEPT );

        ::SetUnhandledExceptionFilter( &Win32SilentUnhandledExceptionFilter );

#if defined(MENGINE_COMPILER_MSVC)
        // Disable both: the "abort message" output and the WER report call
        // performed by the CRT inside abort().
        ::_set_abort_behavior( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );

#   if defined(MENGINE_DEBUG)
        // Redirect _CrtDbgReport (used by debug-CRT asserts) to stderr
        // instead of the modal "Microsoft Visual C++ Debug Library" dialog.
        ::_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
        ::_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
        ::_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
        ::_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
        ::_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
        ::_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
#   endif
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    // Crude but sufficient cmdline scan: we only need this BEFORE any
    // service / option parser exists, just to learn whether the host
    // explicitly asked for silent mode. The real, validated option parsing
    // still happens later inside Win32Application::initializeOptionsService_.
    //////////////////////////////////////////////////////////////////////////
    static bool Win32CmdLineHasCLI( LPSTR _lpCmdLine )
    {
        if( _lpCmdLine == NULL )
        {
            return false;
        }

        if( Mengine::StdString::strstr( _lpCmdLine, "cli" ) == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( nShowCmd );

    Mengine::Configuration configuration;

    bool CLI = Win32CmdLineHasCLI( lpCmdLine );

    if( CLI == true )
    {
        // CLI mode currently implies silent-dialog behaviour. Other CLI-driven
        // configuration knobs can be added here in the future, and silentDialog
        // can also be enabled independently of CLI.
        configuration.silentDialog = true;

        // CLI mode also implies a hidden main window: the engine still
        // initializes (services, scripts, audio) but no visible window is
        // ever shown. Can also be enabled independently of CLI.
        configuration.hideWindow = true;

        // Suppress CRT/SEH/WER popups for this EXE module BEFORE the
        // Mengine DLL/static lib is touched. The DLL gets the same
        // configuration through ServiceProvider once Win32Application::
        // initialize forwards it to API_MengineCreate.
        Win32ApplyCLIPlatform();
    }

    Mengine::Win32Application app;

    if( app.initialize( configuration ) == false )
    {
        if( configuration.silentDialog == false )
        {
            ::MessageBoxA( NULL, "Invalid initialization", "Mengine", MB_OK );
        }

        app.finalize();

        return EXIT_FAILURE;
    }

    app.loop();

    app.finalize();

    return EXIT_SUCCESS;
}