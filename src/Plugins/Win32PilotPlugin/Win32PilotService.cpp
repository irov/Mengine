#include "Win32PilotService.h"

#include "Win32PilotLogger.h"
#include "Win32PilotAnalyticsEventProvider.h"

#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/AllocatorHelper.h"

#include "Pilot/PilotSdk.h"
#include "Pilot/PilotConfig.h"
#include "Pilot/PilotAllocator.h"
#include "Pilot/PilotLogConfigBuilder.h"
#include "Pilot/PilotMetricConfigBuilder.h"
#include "Pilot/PilotSessionAttributeBuilder.h"

SERVICE_FACTORY( Win32PilotService, Mengine::Win32PilotService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * s_pilotMalloc( size_t _size )
    {
        return Helper::allocateMemory( _size, "pilot" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_pilotFree( void * _ptr )
    {
        Helper::deallocateMemory( _ptr, "pilot" );
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PilotService::Win32PilotService()
        : m_sdkInitialized( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PilotService::~Win32PilotService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PilotService::_initializeService()
    {
        URLString Win32PilotPlugin_ApiUrl = CONFIG_VALUE_URLSTRING( "Win32PilotPlugin", "ApiUrl", "" );
        URLString Win32PilotPlugin_ApiToken = CONFIG_VALUE_URLSTRING( "Win32PilotPlugin", "ApiToken", "" );

        if( Win32PilotPlugin_ApiToken.empty() == true )
        {
            LOGGER_WARNING( "Pilot API token not configured, plugin disabled" );

            return true;
        }

        if( Win32PilotPlugin_ApiUrl.empty() == true )
        {
            LOGGER_WARNING( "Pilot API URL not configured, plugin disabled" );

            return true;
        }

        LOGGER_MESSAGE( "Initializing Pilot SDK v%s (server: %s)"
            , Pilot::PilotSdk::VERSION
            , Win32PilotPlugin_ApiUrl.c_str()
        );

        Pilot::pilot_set_allocator( &s_pilotMalloc, &s_pilotFree );

        Char install_id[MENGINE_ENVIRONMENT_INSTALLID_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallId( install_id );

        int64_t install_timestamp = ENVIRONMENT_SERVICE()
            ->getInstallTimestamp();

        Char install_version[MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallVersion( install_version );

        int64_t install_rnd = ENVIRONMENT_SERVICE()
            ->getInstallRND();

        int64_t session_index = ENVIRONMENT_SERVICE()
            ->getSessionIndex();

        int64_t session_timestamp = ENVIRONMENT_SERVICE()
            ->getSessionTimestamp();

        Pilot::PilotSessionAttributeBuilder sessionAttrs;
        sessionAttrs.put( "is_publish", Pilot::PilotString( MENGINE_BUILD_PUBLISH_VALUE( "true", "false" ) ) );
        sessionAttrs.put( "is_debug", Pilot::PilotString( MENGINE_DEBUG_VALUE( "true", "false" ) ) );
        sessionAttrs.put( "install_id", Pilot::PilotString( install_id ) );
        sessionAttrs.put( "install_timestamp", Pilot::pilotToString( install_timestamp ) );
        sessionAttrs.put( "install_version", Pilot::PilotString( install_version ) );
        sessionAttrs.put( "install_rnd", Pilot::pilotToString( install_rnd ) );
        sessionAttrs.put( "session_index", Pilot::pilotToString( session_index ) );
        sessionAttrs.put( "session_timestamp", Pilot::pilotToString( session_timestamp ) );

        Pilot::PilotLogConfigBuilder logConfig;
        logConfig.setEnabled( true );

#if defined(MENGINE_DEBUG)
        logConfig.setLogLevel( Pilot::PilotLogLevel::PL_DEBUG );
#else
        logConfig.setLogLevel( Pilot::PilotLogLevel::PL_INFO );
#endif

        Pilot::PilotMetricConfigBuilder metricConfig;
        metricConfig.setEnabled( true );
        metricConfig.setSampleIntervalMs( 200 );

        Pilot::PilotConfig config = Pilot::PilotConfig::Builder(
            Pilot::PilotString( Win32PilotPlugin_ApiUrl.c_str() ),
            Pilot::PilotString( Win32PilotPlugin_ApiToken.c_str() )
        )
            .setSessionAttributes( sessionAttrs )
            .setLogConfig( logConfig )
            .setMetricConfig( metricConfig )
            .setLoggerListener( &m_pilotLoggerListener )
            .setAutoConnect( false )
            .build();

        Pilot::PilotSdk::initialize( config );
        m_sdkInitialized = true;

        // Register logger to forward engine logs to Pilot
        Win32PilotLoggerPtr pilotLogger = Helper::makeFactorableUnique<Win32PilotLogger>( MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        pilotLogger->setVerboseLevel( LM_ERROR );
#else
        pilotLogger->setVerboseLevel( LM_MESSAGE );
#endif

        pilotLogger->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( pilotLogger ) == true )
        {
            m_pilotLogger = pilotLogger;
        }

        // Register analytics event provider
        Win32PilotAnalyticsEventProviderPtr analyticsProvider = Helper::makeFactorableUnique<Win32PilotAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( analyticsProvider );

        m_analyticsEventProvider = analyticsProvider;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &Win32PilotService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_UPDATE, &Win32PilotService::notifyPlatformUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        // Connect after initialization
        Pilot::PilotSdk::connect();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_UPDATE );

        if( m_analyticsEventProvider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_analyticsEventProvider );

            m_analyticsEventProvider = nullptr;
        }

        if( m_pilotLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_pilotLogger );

            m_pilotLogger = nullptr;
        }

        if( m_sdkInitialized == true )
        {
            Pilot::PilotSdk::shutdown();
            m_sdkInitialized = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotService::notifyPlatformUpdate_()
    {
        if( m_sdkInitialized == false )
        {
            return;
        }

        Pilot::PilotSdk::update();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotService::notifyBootstrapperCreateApplication_()
    {
        if( m_sdkInitialized == false )
        {
            return;
        }

        Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        Char projectVersionString[32 + 1] = {'\0'};
        Helper::stringalized( projectVersion, projectVersionString, 32 );

        bool debugMode = Helper::isDebugMode();
        bool publishMode = Helper::isBuildPublish();

        Pilot::PilotMap<Pilot::PilotString, Pilot::PilotJson> metadata;
        metadata["company"] = Pilot::PilotJson( Pilot::PilotString( companyName ) );
        metadata["project"] = Pilot::PilotJson( Pilot::PilotString( projectName ) );
        metadata["version"] = Pilot::PilotJson( Pilot::PilotString( projectVersionString ) );
        metadata["debug"] = Pilot::PilotJson( debugMode );
        metadata["publish"] = Pilot::PilotJson( publishMode );

        Pilot::PilotSdk::event( "application_created", metadata );
    }
    //////////////////////////////////////////////////////////////////////////
}
