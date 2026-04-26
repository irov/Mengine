#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Configuration.h"

#include "GDKApplication.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace
{
    //////////////////////////////////////////////////////////////////////////
    // Crude but sufficient cmdline scan: we only need this BEFORE any
    // service / option parser exists, just to learn whether the host
    // explicitly asked for silent mode.
    //////////////////////////////////////////////////////////////////////////
    static bool GDKCmdLineHasCLI( LPSTR _lpCmdLine )
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
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    Mengine::Configuration configuration;

    bool CLI = GDKCmdLineHasCLI( lpCmdLine );

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

        // CLI mode also implies the engine update loop must keep running
        // even when the (hidden) main window has no focus: an automated
        // agent driving the process via Win32MessagePipeBridge / DnD probes
        // would otherwise deadlock waiting for the engine to tick. Can also
        // be enabled independently of CLI.
        configuration.nopause = true;

        // CLI mode must not bounce into an already-open interactive instance:
        // the agent needs an independent hidden process with its own message
        // pipe, even while the user keeps a normal game window open.
        configuration.noAlreadyRunning = true;
    }

    Mengine::GDKApplication app;

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