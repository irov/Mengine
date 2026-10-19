#include "WebViewBootstrapper.h"

#include "Interface/LifecycleServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/NotificationHelper.h"

#include "Configuration/Configurations.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewBootstrapper, Mengine::WebViewBootstrapper );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( LifecycleService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( StatisticService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( TimerService );
SERVICE_EXTERN( TimepipeService );
SERVICE_EXTERN( UpdateService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( WebViewBridgeService );
#if defined(MENGINE_PLATFORM_WINDOWS)
SERVICE_EXTERN( EnvironmentService );
#endif
#if defined(MENGINE_SYSTEM_DATETIME)
SERVICE_EXTERN( DateTimeSystem );
#endif
#if defined(MENGINE_SYSTEM_FILE)
SERVICE_EXTERN( FileSystem );
#endif
#if defined(MENGINE_SYSTEM_CRYPTOGRAPHY)
SERVICE_EXTERN( CryptographySystem );
#endif
#if defined(MENGINE_SYSTEM_HTTP)
SERVICE_EXTERN( HttpSystem );
#endif
#if defined(MENGINE_SYSTEM_HTTP)
SERVICE_EXTERN( HttpService );
#endif
#if defined(MENGINE_SYSTEM_SOCKET)
SERVICE_EXTERN( SocketSystem );
#endif
#if defined(MENGINE_SYSTEM_SOCKET)
SERVICE_EXTERN( SocketService );
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WebViewBootstrapper::WebViewBootstrapper()
        : m_context{0, 0.f, 0.f}
        , m_state( STATE_CREATED )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebViewBootstrapper::~WebViewBootstrapper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBootstrapper::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBootstrapper::_finalizeService()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBootstrapper::initialize()
    {
        if( m_state != STATE_CREATED )
        {
            return false;
        }

        if( this->createServices_() == false )
        {
            this->finalize();

            return false;
        }

        m_state = STATE_INITIALIZED;

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_COMPLETE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBootstrapper::run()
    {
        if( m_state != STATE_INITIALIZED )
        {
            return false;
        }

#if defined(MENGINE_SYSTEM_HTTP)
        if( FILE_SERVICE()
            ->hasFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr ) == false )
        {
            return false;
        }
#endif

        THREAD_SERVICE()
            ->updateMainThread();

        if( this->createNetworkServices_() == false )
        {
            this->stop();

            return false;
        }

        m_state = STATE_RUNNING;

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN );
        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBootstrapper::tick( float _time )
    {
        if( m_state != STATE_RUNNING )
        {
            return;
        }

        MENGINE_ASSERTION_MAIN_THREAD_GUARD();

        LIFECYCLE_SERVICE()
            ->preUpdate();

        if( m_state != STATE_RUNNING )
        {
            return;
        }

        LIFECYCLE_SERVICE()
            ->update();

        if( m_state != STATE_RUNNING )
        {
            return;
        }

        ++m_context.revision;
        m_context.time = _time;

        TIMEPIPE_SERVICE()
            ->tick( &m_context );

        m_context.current += _time;

        if( m_state != STATE_RUNNING )
        {
            return;
        }

        LIFECYCLE_SERVICE()
            ->postUpdate();
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBootstrapper::stop()
    {
        if( m_state == STATE_STOPPED )
        {
            return;
        }

        m_state = STATE_STOPPED;

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP );
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBootstrapper::finalize()
    {
        if( m_services.empty() == true )
        {
            return;
        }

        this->stop();

        SERVICE_PROVIDER_GET()
            ->stopServices();

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP_SERVICES );
        }

        if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();

            THREAD_SERVICE()
                ->stopDispatching();
        }

        if( SERVICE_IS_INITIALIZE( PluginServiceInterface ) == true )
        {
            PLUGIN_SERVICE()
                ->unloadPlugins();
        }

        for( VectorServiceNames::const_reverse_iterator it = m_services.rbegin(); it != m_services.rend(); ++it )
        {
            SERVICE_PROVIDER_GET()
                ->finalizeService( *it );

            SERVICE_PROVIDER_GET()
                ->destroyService( *it );
        }

        m_services.clear();
    }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_SERVICE( Name, Doc )\
    {\
        const Char * name = SERVICE_FUNCTION_NOMINATION( Name )();\
        m_services.emplace_back( name );\
        if( SERVICE_CREATE_SAFE( Name, Doc ) == false )\
        {\
            return false;\
        }\
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBootstrapper::createServices_()
    {
        MENGINE_ADD_SERVICE( ThreadSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimeSystem, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_SYSTEM_DATETIME)
        MENGINE_ADD_SERVICE( DateTimeSystem, MENGINE_DOCUMENT_FACTORABLE );
#endif

        MENGINE_ADD_SERVICE( LifecycleService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( NotificationService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( VocabularyService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( OptionsService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FactoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PrototypeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( MemoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( StatisticService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( LoggerService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ConfigService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ThreadService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FileService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ResourceService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimerService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimepipeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( UpdateService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PluginService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( WebViewBridgeService, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_SYSTEM_FILE)
        MENGINE_ADD_SERVICE( FileSystem, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_SYSTEM_CRYPTOGRAPHY)
        MENGINE_ADD_SERVICE( CryptographySystem, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
        MENGINE_ADD_SERVICE( EnvironmentService, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBootstrapper::createNetworkServices_()
    {
#if defined(MENGINE_SYSTEM_HTTP)
        MENGINE_ADD_SERVICE( HttpSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( HttpService, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_SYSTEM_SOCKET)
        MENGINE_ADD_SERVICE( SocketSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( SocketService, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }

#undef MENGINE_ADD_SERVICE
    //////////////////////////////////////////////////////////////////////////
}
