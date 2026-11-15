#include "Win32OpButtonService.h"

#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Win32OpButtonAnalyticsEventProvider.h"
#include "Win32OpButtonLogger.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Stringalized.h"

#include "OpButton/OpButtonAllocator.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonConfig.h"
#include "OpButton/OpButtonLogConfigBuilder.h"
#include "OpButton/OpButtonMetricConfigBuilder.h"
#include "OpButton/OpButtonSdk.h"
#include "OpButton/OpButtonSessionAttributeBuilder.h"
#include "OpButton/OpButtonTab.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Win32OpButtonService, Mengine::Win32OpButtonService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const uint32_t OPBUTTON_METRIC_SAMPLE_INTERVAL_MILLISECONDS = 200;
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * opButtonMalloc( size_t _size )
        {
            void * memory = Helper::allocateMemory( _size, "opbutton" );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        static void opButtonFree( void * _ptr )
        {
            Helper::deallocateMemory( _ptr, "opbutton" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonService::Win32OpButtonService()
        : m_sdkInitialized( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32OpButtonService::~Win32OpButtonService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & Win32OpButtonService::requiredServices() const
    {
        static const ServiceRequiredList required = {
            SERVICE_ID( AnalyticsServiceInterface ),
            SERVICE_ID( EnvironmentServiceInterface ),
            SERVICE_ID( LoggerServiceInterface ),
            SERVICE_ID( NotificationServiceInterface ),
            SERVICE_ID( ConfigServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonService::addCommandButton( const ConstString & _tab, const ConstString & _identifier, const ConstString & _title, const Lambda<void()> & _callback )
    {
        if( m_sdkInitialized == false )
        {
            return false;
        }

        CommandTabDesc & desc = m_commandTabs[_tab];

        if( desc.layout == nullptr )
        {
            OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();
            const Char * name = _tab.c_str();
            OpButton::OpButtonTab & tab = ui.addTab( name );
            tab.setId( name );
            desc.layout = &tab.vertical();
        }

        const Char * title = _title.c_str();
        OpButton::OpButtonButton & button = desc.layout->addButton( title );
        const Char * identifier = _identifier.c_str();
        button.setId( identifier );

        OpButton::OpButtonWidgetCallbackFunc handler = [callback = _callback]( const OpButton::OpButtonAction & _action ) {
            MENGINE_UNUSED( _action );

            callback();
        };

        button.onClick( handler );
        desc.buttons.emplace_back( &button );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonService::removeCommandTab( const ConstString & _tab )
    {
        MapCommandTabs::iterator itTab = m_commandTabs.find( _tab );

        if( itTab == m_commandTabs.end() )
        {
            return;
        }

        VectorCommandButtons buttons;
        buttons.swap( itTab->second.buttons );
        m_commandTabs.erase( itTab );

        for( OpButton::OpButtonButton * button : buttons )
        {
            OpButton::OpButtonWidgetCallbackFunc callback;
            button->onClick( callback );
        }

        OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();
        const Char * name = _tab.c_str();
        ui.removeTab( name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonService::_initializeService()
    {
        URLString apiUrl = CONFIG_VALUE_URLSTRING( "Win32OpButtonPlugin", "ApiUrl", "" );
        URLString apiToken = CONFIG_VALUE_URLSTRING( "Win32OpButtonPlugin", "ApiToken", "" );

        if( apiToken.empty() == true )
        {
            LOGGER_WARNING( "OpButton API token not configured, plugin disabled" );

            return true;
        }

        if( apiUrl.empty() == true )
        {
            LOGGER_WARNING( "OpButton API URL not configured, plugin disabled" );

            return true;
        }

        LOGGER_MESSAGE( "Initializing OpButton SDK v%s (server: %s)"
            , OpButton::OpButtonSdk::VERSION
            , apiUrl.c_str()
        );

        OpButton::opbutton_set_allocator( &Detail::opButtonMalloc, &Detail::opButtonFree );

        Char installId[MENGINE_ENVIRONMENT_INSTALLID_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallId( installId );

        int64_t installTimestamp = ENVIRONMENT_SERVICE()
            ->getInstallTimestamp();

        Char installVersion[MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallVersion( installVersion );

        int64_t installRND = ENVIRONMENT_SERVICE()
            ->getInstallRND();

        int64_t sessionIndex = ENVIRONMENT_SERVICE()
            ->getSessionIndex();

        int64_t sessionTimestamp = ENVIRONMENT_SERVICE()
            ->getSessionTimestamp();

        OpButton::OpButtonSessionAttributeBuilder sessionAttrs;
        OpButton::OpButtonString publishAttribute( MENGINE_BUILD_PUBLISH_VALUE( "true", "false" ) );
        sessionAttrs.put( "is_publish", publishAttribute );
        OpButton::OpButtonString debugAttribute( MENGINE_DEBUG_VALUE( "true", "false" ) );
        sessionAttrs.put( "is_debug", debugAttribute );
        OpButton::OpButtonString installIdAttribute( installId );
        sessionAttrs.put( "install_id", installIdAttribute );
        OpButton::OpButtonString installTimestampAttribute = OpButton::opbuttonToString( installTimestamp );
        sessionAttrs.put( "install_timestamp", installTimestampAttribute );
        OpButton::OpButtonString installVersionAttribute( installVersion );
        sessionAttrs.put( "install_version", installVersionAttribute );
        OpButton::OpButtonString installRNDAttribute = OpButton::opbuttonToString( installRND );
        sessionAttrs.put( "install_rnd", installRNDAttribute );
        OpButton::OpButtonString sessionIndexAttribute = OpButton::opbuttonToString( sessionIndex );
        sessionAttrs.put( "session_index", sessionIndexAttribute );
        OpButton::OpButtonString sessionTimestampAttribute = OpButton::opbuttonToString( sessionTimestamp );
        sessionAttrs.put( "session_timestamp", sessionTimestampAttribute );

        OpButton::OpButtonLogConfigBuilder logConfig;
        logConfig.setEnabled( true );

#if defined(MENGINE_DEBUG)
        logConfig.setLogLevel( OpButton::OpButtonLogLevel::PL_DEBUG );
#else
        logConfig.setLogLevel( OpButton::OpButtonLogLevel::PL_INFO );
#endif

        OpButton::OpButtonMetricConfigBuilder metricConfig;
        metricConfig.setEnabled( true );
        metricConfig.setSampleIntervalMs( OPBUTTON_METRIC_SAMPLE_INTERVAL_MILLISECONDS );

        const Char * apiUrlValue = apiUrl.c_str();
        const Char * apiTokenValue = apiToken.c_str();
        OpButton::OpButtonString opButtonApiUrl( apiUrlValue );
        OpButton::OpButtonString opButtonApiToken( apiTokenValue );
        OpButton::OpButtonConfig config = OpButton::OpButtonConfig::Builder( opButtonApiUrl, opButtonApiToken )
            .setSessionAttributes( sessionAttrs )
            .setLogConfig( logConfig )
            .setMetricConfig( metricConfig )
            .setLoggerListener( &m_opbuttonLoggerListener )
            .setAutoConnect( false )
            .build();

        OpButton::OpButtonSdk::initialize( config );
        m_sdkInitialized = true;

        Win32OpButtonLoggerPtr opbuttonLogger = Helper::makeFactorableUnique<Win32OpButtonLogger>( MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        opbuttonLogger->setVerboseLevel( LM_ERROR );
#else
        opbuttonLogger->setVerboseLevel( LM_MESSAGE );
#endif

        opbuttonLogger->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( opbuttonLogger ) == true )
        {
            m_opbuttonLogger = opbuttonLogger;
        }

        Win32OpButtonAnalyticsEventProviderPtr analyticsProvider = Helper::makeFactorableUnique<Win32OpButtonAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( analyticsProvider );

        m_analyticsEventProvider = analyticsProvider;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &Win32OpButtonService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_UPDATE, &Win32OpButtonService::notifyPlatformUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        OpButton::OpButtonSdk::connect();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonService::_finalizeService()
    {
        if( m_sdkInitialized == false )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_UPDATE );

        while( m_commandTabs.empty() == false )
        {
            ConstString tab = m_commandTabs.begin()->first;
            this->removeCommandTab( tab );
        }

        if( m_analyticsEventProvider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_analyticsEventProvider );

            m_analyticsEventProvider = nullptr;
        }

        if( m_opbuttonLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_opbuttonLogger );

            m_opbuttonLogger = nullptr;
        }

        OpButton::OpButtonSdk::shutdown();
        m_sdkInitialized = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonService::notifyPlatformUpdate_()
    {
        if( m_sdkInitialized == false )
        {
            return;
        }

        OpButton::OpButtonSdk::update();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonService::notifyBootstrapperCreateApplication_()
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

        constexpr size_t projectVersionCapacity = 32;
        Char projectVersionString[projectVersionCapacity + 1] = {'\0'};
        Helper::stringalized( projectVersion, projectVersionString, projectVersionCapacity );

        bool debugMode = Helper::isDebugMode();
        bool publishMode = Helper::isBuildPublish();

        typedef OpButton::OpButtonMap<OpButton::OpButtonString, OpButton::OpButtonJson> OpButtonMetadata;
        OpButtonMetadata metadata;
        OpButton::OpButtonString companyNameValue( companyName );
        metadata["company"] = OpButton::OpButtonJson( companyNameValue );
        OpButton::OpButtonString projectNameValue( projectName );
        metadata["project"] = OpButton::OpButtonJson( projectNameValue );
        OpButton::OpButtonString projectVersionValue( projectVersionString );
        metadata["version"] = OpButton::OpButtonJson( projectVersionValue );
        metadata["debug"] = OpButton::OpButtonJson( debugMode );
        metadata["publish"] = OpButton::OpButtonJson( publishMode );

        OpButton::OpButtonSdk::event( "application_created", metadata );
    }
    //////////////////////////////////////////////////////////////////////////
}
