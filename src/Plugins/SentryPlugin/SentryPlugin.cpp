#include "SentryPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/Stringalized.h"

#include "sentry.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Sentry, Mengine::SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::~SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_initializePlugin()
    {
        sentry_options_t * options = sentry_options_new();
        sentry_options_set_dsn( options, "https://fbdebd6adfef416f988343a18124aa32@sentry.io/2491034" );
        sentry_options_set_handler_path( options, "crashpad_handler.exe" );
        sentry_options_set_debug( options, 1 );
        sentry_init( options );

        sentry_set_extra( "Application", sentry_value_new_string( "Mengine" ) );

        Char companyName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME];
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        sentry_set_extra( "Company", sentry_value_new_string( companyName ) );

        Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME];
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        sentry_set_extra( "Project", sentry_value_new_string( projectName ) );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32];
        Helper::stringalized( projectVersion, projectVersionString, 32 );

        sentry_set_extra( "Version", sentry_value_new_string( projectVersionString ) );
        sentry_set_extra( "GitHub SHA1", sentry_value_new_string( MENGINE_GITHUB_SHA1 ) );

        Char releaseString[32];
        sprintf( releaseString, "%s@%u", projectName, projectVersion );

        sentry_options_set_release( options, releaseString );

        //sentry_capture_event( sentry_value_new_message_event(
        //    /*   level */ SENTRY_LEVEL_INFO,
        //    /*  logger */ "custom",
        //    /* message */ "It works!"
        //) );

        //*(int *)(0) = 0;
        //memset( (char *)0x0, 1, 100 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_finalizePlugin()
    {
        sentry_shutdown();
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_destroyPlugin()
    {
    }
}
