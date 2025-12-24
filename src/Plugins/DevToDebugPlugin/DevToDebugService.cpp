#include "DevToDebugService.h"

#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/TimerServiceInterface.h"
#include "Interface/HttpServiceInterface.h"
#include "Interface/HttpRequestInterface.h"

#include "DevToDebugTab.h"

#include "DevToDebugPropertyConstBoolean.h"
#include "DevToDebugPropertyGetterBoolean.h"
#include "DevToDebugPropertyConstString.h"
#include "DevToDebugPropertyGetterString.h"
#include "DevToDebugPropertyConstColor.h"
#include "DevToDebugPropertyGetterColor.h"
#include "DevToDebugPropertyContent.h"

#include "DevToDebugWidget.h"
#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"
#include "DevToDebugWidgetCheckbox.h"
#include "DevToDebugWidgetCommandLine.h"
#include "DevToDebugWidgetRadioButton.h"
#include "DevToDebugWidgetSelector.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/Fingerprint.h"
#include "Kernel/DataHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DevToDebugService, Mengine::DevToDebugService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::DevToDebugService()
        : m_status( EDTDS_NONE )
        , m_revision( 0 )
        , m_timerId( INVALID_UNIQUE_ID )
        , m_invalidateTabs( false )
        , m_throttle( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::~DevToDebugService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & DevToDebugService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( HttpServiceInterface ),
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugService::_initializeService()
    {
        URLString DevToDebug_DSN = CONFIG_VALUE_URLSTRING( "DevToDebugPlugin", "DSN", "" );

        LOGGER_MESSAGE( "DevToDebug DSN: %s"
            , DevToDebug_DSN.c_str()
        );

        m_dsn = DevToDebug_DSN;

        m_mutexTabs = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_mutexCommands = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE, &DevToDebugService::notifyBootstrapperRunComplete_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &DevToDebugService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        if( Helper::addObjectPrototype<DevToDebugTab, 16>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstBoolean, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterBoolean, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstString, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterString, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyConstColor, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyGetterColor, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<DevToDebugPropertyContent, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetButton, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetText, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetCheckbox, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetCommandLine, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetRadioButton, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addObjectPrototype<DevToDebugWidgetSelector, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        Helper::fingerprintSHA1( m_fingerprint, false );

        uint32_t DevToDebug_ProccesTime = CONFIG_VALUE_INTEGER( "DevToDebugPlugin", "ProccesTime", 2000U );

        ThreadIdentityInterfacePtr thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGD2DProcess" ), ETP_BELOW_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            this->process( _runner );

            return true;
        }, DevToDebug_ProccesTime, MENGINE_DOCUMENT_FACTORABLE );

        m_thread = thread;

        Timestamp DevToDebug_PropertySyncTime = CONFIG_VALUE_INTEGER( "DevToDebugPlugin", "PropertySyncTime", 1000ULL );

        UniqueId timerId = TIMER_SERVICE()
            ->addTimer( DevToDebug_PropertySyncTime, [this]( UniqueId _id )
        {
            MENGINE_UNUSED( _id );

            this->sync();
        }, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_FATAL( timerId != INVALID_UNIQUE_ID, "invalid add timer" );

        m_timerId = timerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_finalizeService()
    {
        Helper::removePrototype<DevToDebugTab>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );

        Helper::removePrototype<DevToDebugPropertyConstBoolean>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyConstString>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyConstColor>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyGetterBoolean>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyGetterString>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyGetterColor>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugPropertyContent>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetButton>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetText>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetCheckbox>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetCommandLine>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetRadioButton>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );
        Helper::removePrototype<DevToDebugWidgetSelector>( STRINGIZE_STRING_LOCAL( "DevToDebug" ) );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_stopService()
    {
        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }

        if( m_timerId != INVALID_UNIQUE_ID )
        {
            TIMER_SERVICE()
                ->removeTimer( m_timerId );
            m_timerId = INVALID_UNIQUE_ID;
        }

        m_mutexTabs = nullptr;
        m_mutexCommands = nullptr;

        m_tabsProcess.clear();
        m_tabsSync.clear();

        m_tabsCommands.clear();

        this->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab )
    {
        m_mutexTabs->lock();
        MENGINE_ASSERTION_FATAL( m_tabsProcess.exist( _name ) == false, "already exist DevToDebug tab '%s'"
            , _name.c_str()
        );

        m_tabsProcess.emplace( _name, _tab );

        m_invalidateTabs = true;
        m_mutexTabs->unlock();

        m_tabsSync.emplace( _name, _tab );
    }
    //////////////////////////////////////////////////////////////////////////
    const DevToDebugTabInterfacePtr & DevToDebugService::getTab( const ConstString & _name ) const
    {
        const DevToDebugTabInterfacePtr & tab = m_tabsSync.find( _name );

        return tab;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugService::hasTab( const ConstString & _name ) const
    {
        bool result = m_tabsSync.exist( _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::removeTab( const ConstString & _name )
    {
        m_mutexTabs->lock();
        DevToDebugTabInterfacePtr tab = m_tabsProcess.erase( _name );

        m_invalidateTabs = true;
        m_mutexTabs->unlock();

        tab->finalize();

        m_tabsSync.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::sync()
    {
        for( const HashtableDevToDebugTabs::value_type & value : m_tabsSync )
        {
            const DevToDebugTabInterfacePtr & tab = value.element;

            tab->syncWidgets();
        }

        m_mutexCommands->lock();
        for( const DevToDebugWidgetCommand & command : m_tabsCommands )
        {
            command();
        }
        m_tabsCommands.clear();
        m_mutexCommands->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::process( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        MENGINE_UNUSED( _runner );

        switch( m_status )
        {
        case EDTDS_NONE:
            break;
        case EDTDS_READY:
            {
                m_status = EDTDS_REGISTRATING;

                const HttpHeaders & headers = HTTP_SERVICE()
                    ->getApplicationJSONHeaders();

                jpp::object j = this->makeJsonRegistrationData();

                Data data;
                Helper::writeJSONDataCompact( j, &data );

                HttpRequestId id = HTTP_SERVICE()
                    ->headerData( m_dsn, headers, data, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        case EDTDS_REGISTRATION:
            {
                m_status = EDTDS_WAITING;

                const HttpHeaders & headers = HTTP_SERVICE()
                    ->getApplicationJSONHeaders();

                HttpRequestId id = HTTP_SERVICE()
                    ->getMessage( m_workerURL, headers, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        case EDTDS_WAIT:
            {
                m_status = EDTDS_CONNECTING;

                const HttpHeaders & headers = HTTP_SERVICE()
                    ->getApplicationJSONHeaders();

                jpp::object j = this->makeJsonConnectData();

                Data data;
                Helper::writeJSONDataCompact( j, &data );

                HttpRequestId id = HTTP_SERVICE()
                    ->headerData( m_workerURL, headers, data, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        case EDTDS_CONNECT:
            {
                const HttpHeaders & headers = HTTP_SERVICE()
                    ->getApplicationJSONHeaders();

                jpp::object j = this->makeJsonProcessData();

                Data data;
                Helper::writeJSONDataCompact( j, &data );

                HttpRequestId id = HTTP_SERVICE()
                    ->headerData( m_workerURL, headers, data, 2000, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_UNUSED( id );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        const HttpRequestInterfacePtr & request = _response->getRequest();
        HttpRequestId requestId = request->getRequestId();

        bool responseSuccessful = _response->isSuccessful();
        EHttpCode responseCode = _response->getCode();
        const String & responseErrorMessage = _response->getErrorMessage();
        int32_t responseErrorCode = _response->getErrorCode();

        const String & responseJson = _response->getJSON();

        switch( m_status )
        {
        case EDTDS_NONE:
            break;
        case EDTDS_REGISTRATING:
            {
                if( responseSuccessful == false )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                if( responseCode / 100 != 2 )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] data: %s [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                jpp::object j = Helper::loadJSONString( responseJson, MENGINE_DOCUMENT_FACTORABLE );

                m_workerURL = j.get( "url", "" );

                if( m_workerURL.empty() == true )
                {
                    LOGGER_ERROR( "connecting response error: %s [code %u] [id %u]"
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                m_throttle = j.get( "throttle" );

                LOGGER_INFO( "devtodebug", "request connect: %s [id %u]"
                    , m_workerURL.c_str()
                    , requestId
                );

                m_status = EDTDS_REGISTRATION;
            }break;
        case EDTDS_WAITING:
            {
                if( responseSuccessful == false )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                if( responseCode / 100 != 2 )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] data: %s [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                jpp::object j = Helper::loadJSONString( responseJson, MENGINE_DOCUMENT_FACTORABLE );

                bool is_watched = j.get( "is_watched" );

                if( is_watched == true )
                {
                    m_status = EDTDS_WAIT;
                }
                else
                {
                    m_status = EDTDS_REGISTRATION;
                }
            }break;
        case EDTDS_CONNECTING:
            {
                if( responseSuccessful == false )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                if( responseCode / 100 != 2 )
                {
                    LOGGER_ERROR( "connecting error: %s [%d] data: %s [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                LOGGER_INFO( "devtodebug", "request connect: %s [id %u]"
                    , m_workerURL.c_str()
                    , requestId
                );

                m_status = EDTDS_CONNECT;
            }break;
        case EDTDS_CONNECT:
            {
                if( responseSuccessful == false )
                {
                    LOGGER_ERROR( "connect response error: %s [%d] [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                if( responseCode / 100 != 2 )
                {
                    LOGGER_ERROR( "connect response error: %s [%d] data: %s [code %u] [id %u]"
                        , responseErrorMessage.c_str()
                        , responseErrorCode
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

                jpp::object j = Helper::loadJSONString( responseJson, MENGINE_DOCUMENT_FACTORABLE );

                uint32_t revision_from = j.get( "revision_from", 0U );
                uint32_t revision_to = j.get( "revision_to", 0U );

                if( revision_from == revision_to )
                {
                    break;
                }

                if( m_revision == revision_to )
                {
                    break;
                }

                if( m_revision != revision_from )
                {
                    LOGGER_ERROR( "connect out of sync revision %u != from %u: %s [code %u] [id %u]"
                        , m_revision
                        , revision_from
                        , responseJson.c_str()
                        , responseCode
                        , requestId
                    );

                    this->stop();

                    break;
                }

#if defined(MENGINE_DEBUG)
                String json_data;
                Helper::writeJSONStringCompact( j, &json_data );

                LOGGER_INFO( "devtodebug", "request process: %s [id %u]"
                    , json_data.c_str()
                    , requestId
                );
#endif

                m_revision = revision_to;

                jpp::array activity = j.get( "activity" );

                for( const jpp::object & a : activity )
                {
                    ConstString tab_name = a.get( "tab_name" );
                    ConstString id = a.get( "id" );
                    jpp::object d = a.get( "data" );

                    m_mutexTabs->lock();
                    DevToDebugTabInterfacePtr tab = m_tabsProcess.find( tab_name );
                    m_mutexTabs->unlock();

                    if( tab == nullptr )
                    {
                        continue;
                    }

                    const DevToDebugWidgetInterfacePtr & widget = tab->findWidget( id );

                    DevToDebugWidgetPtr widget_base = DevToDebugWidgetPtr::dynamic_from( widget );

                    widget_base->process( d, m_mutexCommands, &m_tabsCommands );
                }
            }break;
        default:
            break;
        }

        return;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::stop()
    {
        m_status = EDTDS_NONE;
        m_revision = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonTabs( bool _force, bool * const _invalidateTabs )
    {
        m_mutexTabs->lock();

        jpp::object jtabs = jpp::make_object();

        bool invalidateTabs = false;

        for( const HashtableDevToDebugTabs::value_type & value : m_tabsProcess )
        {
            const ConstString & key = value.key;
            const DevToDebugTabInterfacePtr & tab = value.element;

            jpp::object jtab = jpp::make_object();

            jpp::array jwidgets = jpp::make_array();

            bool invalidateWidgets;
            tab->foreachWidgets( [&jwidgets, _force]( const DevToDebugWidgetInterfacePtr & _widget )
            {
                DevToDebugWidgetPtr widget = DevToDebugWidgetPtr::dynamic_from( _widget );

                jpp::object jwidget = jpp::make_object();

                if( widget->fillJson( jwidget, _force ) == true )
                {
                    jwidgets.push_back( jwidget );
                }
            }, &invalidateWidgets );

            invalidateTabs |= invalidateWidgets;

            if( jwidgets.empty() == false )
            {
                jtab.set( "widgets", jwidgets );

                jtabs.set( key, jtab );
            }
        }

        *_invalidateTabs = invalidateTabs || m_invalidateTabs;

        m_invalidateTabs = false;

        m_mutexTabs->unlock();

        return jtabs;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonRegistrationData()
    {
        jpp::object j = jpp::make_object();

        j.set( "did", m_fingerprint );

        Char userName[MENGINE_ENVIRONMENT_USER_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getUserName( userName );

        j.set( "name", userName );

        LOGGER_INFO( "devtodebug", "device did '%s' name '%s'"
            , m_fingerprint
            , userName
        );

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonWaitData()
    {
        jpp::object j = jpp::make_object();

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonConnectData()
    {
        jpp::object j = jpp::make_object();

        jpp::object jstate = jpp::make_object();

        bool invalidateTabs;
        jpp::object jtabs = this->makeJsonTabs( true, &invalidateTabs );

        jstate.set( "tabs", jtabs );

        jpp::object jworker = jpp::make_object();

        jworker.set( "new_state", jstate );

        j.set( "worker", jworker );

        m_invalidateTabs = false;

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeJsonProcessData()
    {
        jpp::object j = jpp::make_object();

        jpp::object jworker = jpp::make_object();

        jworker.set( "confirmed_revision", m_revision );

        jpp::object jstate = jpp::make_object();

        bool invalidateTabs;
        jpp::object jtabs = this->makeJsonTabs( false, &invalidateTabs );

        if( jtabs.empty() == false )
        {
            jstate.set( "tabs", jtabs );

            if( m_invalidateTabs == true || invalidateTabs == true )
            {
                jworker.set( "new_state", jstate );
            }
            else
            {
                jworker.set( "change_state", jstate );
            }
        }

#if defined(MENGINE_DEBUG)
        static uint32_t old_revision = 0;

        if( m_revision != old_revision )
        {
            LOGGER_INFO( "devtodebug", "confirmed revision: %u"
                , m_revision
            );

            String data;
            Helper::writeJSONStringCompact( jstate, &data );

            LOGGER_INFO( "devtodebug", "change state: %s"
                , data.c_str()
            );

            old_revision = m_revision;
        }
#endif

        j.set( "worker", jworker );

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperRunComplete_()
    {
        m_status = EDTDS_READY;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperFinalizeGame_()
    {
        if( m_workerURL.empty() == true )
        {
            return;
        }

        HttpHeaders headers;

        HTTP_SERVICE()
            ->deleteMessage( m_workerURL, headers, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, EHRF_LOW_PRIORITY, nullptr, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
}
