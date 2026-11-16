#include "Win32OpButtonService.h"

#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/Win32KernelServiceInterface.h"

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

#include "Config/StdException.h"

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

        OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();
        const Char * identifierTab = _tab.c_str();
        OpButton::OpButtonTab & tab = ui.getOrCreateTab( identifierTab );
        OpButton::OpButtonLayout * layout = tab.getLayout();

        if( layout == nullptr )
        {
            layout = &tab.horizontal();
        }

        m_commandTabs.insert( _tab );

        const Char * title = _title.c_str();
        OpButton::OpButtonButton & button = layout->addButton( title );
        const Char * identifier = _identifier.c_str();
        button.setId( identifier );

        OpButton::OpButtonWidgetCallbackFunc handler = [_callback]( const OpButton::OpButtonAction & _action ) {
            MENGINE_UNUSED( _action );

            _callback();
        };

        button.onClick( handler );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonService::setCommandTab( const ConstString & _tab, const String & _layout, const Lambda<void( const String &, const ParamVariant & )> & _callback )
    {
        if( m_sdkInitialized == false )
        {
            return false;
        }

        OpButton::OpButtonJson description = OpButton::OpButtonJson::parse( _layout.c_str() );
        OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();
        const Char * identifierTab = _tab.c_str();
        bool created = ui.getTab( identifierTab ) == nullptr;
        OpButton::OpButtonTab & tab = ui.getOrCreateTab( identifierTab );

        try
        {
            tab.setJson( description, [_callback]( const OpButton::OpButtonString & _identifier, const OpButton::OpButtonAction & _action ) {
                OpButton::OpButtonJson payload = _action.payload().get( "value" );
                ParamVariant value = nullptr;

                if( payload.type() == OpButton::OpButtonJson::TYPE_STRING )
                {
                    OpButton::OpButtonString text = payload.asString();
                    value = String( text.c_str(), text.size() );
                }
                else if( payload.type() == OpButton::OpButtonJson::TYPE_BOOL )
                {
                    value = payload.asBool();
                }
                else if( payload.type() == OpButton::OpButtonJson::TYPE_DOUBLE )
                {
                    value = payload.asDouble();
                }
                else if( payload.type() == OpButton::OpButtonJson::TYPE_INT )
                {
                    value = payload.asInt64();
                }

                String identifier( _identifier.c_str(), _identifier.size() );
                _callback( identifier, value );
            } );
        }
        catch( const StdException::invalid_argument & exception )
        {
            if( created == true )
            {
                ui.removeTab( identifierTab );
            }

            LOGGER_ERROR( "invalid OpButton JSON layout: %s", exception.what() );

            return false;
        }

        m_commandTabs.insert( _tab );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32OpButtonService::removeCommandTab( const ConstString & _tab )
    {
        SetCommandTabs::iterator itTab = m_commandTabs.find( _tab );

        if( itTab == m_commandTabs.end() )
        {
            return;
        }

        m_commandTabs.erase( itTab );

        OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();
        const Char * name = _tab.c_str();
        ui.removeTab( name );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32OpButtonService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_RUN, &Win32OpButtonService::notifyPlatformRun_, MENGINE_DOCUMENT_FACTORABLE );

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
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_RUN );

        if( m_sdkInitialized == false )
        {
            return;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_UPDATE );

        SetCommandTabs commandTabs;
        commandTabs.swap( m_commandTabs );

        OpButton::OpButtonUI & ui = OpButton::OpButtonSdk::getUI();

        for( const ConstString & tab : commandTabs )
        {
            ui.removeTab( tab.c_str() );
        }

        m_commandTabs.clear();

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
    void Win32OpButtonService::notifyPlatformRun_()
    {
        const ConstString & semaphoreName = STRINGIZE_STRING_LOCAL( "OpButtonReady" );

        WIN32_KERNEL_SERVICE()
            ->activateSemaphore( semaphoreName, m_sdkInitialized );
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
