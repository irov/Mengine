#include "DevToDebugService.h"

#include "DevToDebugTab.h"
#include "DevToDebugWidget.h"
#include "DevToDebugWidgetText.h"
#include "DevToDebugWidgetButton.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/JSONHelper.h"

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
            cURLServiceInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugService::_initializeService()
    {
        const Char * DevToDebug_PID = CONFIG_VALUE( "DevToDebug", "PID", "" );

        if( MENGINE_STRCMP( DevToDebug_PID, "" ) == 0 )
        {
            LOGGER_WARNING( "DevToDebug don't setup PID" );

            return true;
        }

        m_pid = DevToDebug_PID;

        LOGGER_MESSAGE( "DevToDebug PID: %s"
            , m_pid.c_str()
        );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE, &DevToDebugService::notifyBootstrapperRunComplete_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &DevToDebugService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

        if( Helper::addDefaultVirtualInheritancePrototype<DevToDebugTab, 16>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultVirtualInheritancePrototype<DevToDebugWidgetButton, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultVirtualInheritancePrototype<DevToDebugWidgetText, 64>( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_finalizeService()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugTab" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetButton" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "DevToDebug" ), STRINGIZE_STRING_LOCAL( "DevToDebugWidgetText" ), nullptr );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab )
    {
        m_tabs.emplace( _name, _tab );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::removeTab( const ConstString & _name )
    {
        m_tabs.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::process()
    {
        switch( m_status )
        {
        case EDTDS_NONE:
            {
                m_status = EDTDS_CONNECTING;

                Char connect_url[256] = {'\0'};
                MENGINE_SPRINTF( connect_url, "http://www.devtodebug.com/api/%s/connect/"
                    , m_pid.c_str()
                );

                cURLHeaders headers;
                headers.push_back( "Content-Type:application/json" );

                jpp::object j = this->makeConnectData();

                String data;
                Helper::writeJSONString( j, &data );

                CURL_SERVICE()
                    ->headerData( connect_url, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, cURLReceiverInterfacePtr::from( this ) );
            }break;
        case EDTDS_CONNECT:
            {
                Char connect_url[256] = {'\0'};
                MENGINE_SPRINTF( connect_url, "http://www.devtodebug.com/api/worker/%s/"
                    , m_uuid.c_str()
                );

                cURLHeaders headers;
                headers.push_back( "Content-Type:application/json" );

                jpp::object j = this->makeProcessData();

                String data;
                Helper::writeJSONString( j, &data );

                CURL_SERVICE()
                    ->headerData( connect_url, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, cURLReceiverInterfacePtr::from( this ) );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::onHttpRequestComplete( const cURLResponseData & _response )
    {
        switch( m_status )
        {
        case Mengine::EDTDS_NONE:
            break;
        case Mengine::EDTDS_CONNECTING:
            {
                if( _response.successful == false )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    break;
                }

                if( _response.code / 100 != 2 )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    break;
                }

                jpp::object j = Helper::loadJSONStreamFromString( _response.data, MENGINE_DOCUMENT_FACTORABLE );

                m_uuid = j.get( "uuid", "" );

                if( m_uuid.empty() == true )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    break;
                }

                m_status = EDTDS_CONNECT;

                this->process();
            }break;
        case Mengine::EDTDS_CONNECT:
            {
                if( _response.successful == false )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    this->process();

                    break;
                }

                if( _response.code / 100 != 2 )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    this->process();

                    break;
                }

                jpp::object j = Helper::loadJSONStreamFromString( _response.data, MENGINE_DOCUMENT_FACTORABLE );

                uint32_t revision_from = j.get( "revision_from", 0U );
                uint32_t revision_to = j.get( "revision_to", 0U );

                if( m_revision != revision_from )
                {
                    m_status = EDTDS_NONE;
                    m_revision = 0;

                    break;
                }

                m_revision = revision_to;

                jpp::array activity = j.get( "activity" );

                for( const jpp::object & a : activity )
                {
                    ConstString tab_name = a.get( "tab_name" );
                    ConstString id = a.get( "id" );

                    jpp::object d = a.get( "data" );
                    
                    DevToDebugTabInterfacePtr tab = m_tabs.find( tab_name );

                    if( tab == nullptr )
                    {
                        continue;
                    }

                    const DevToDebugWidgetInterfacePtr & widget = tab->findWidget( id );

                    DevToDebugWidgetPtr widget_base = DevToDebugWidgetPtr::dynamic_from( widget );

                    widget_base->process( d );
                }

                this->process();

            }break;
        default:
            break;
        }

        return;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeConnectData()
    {
        jpp::object j = jpp::make_object();

        j.set( "did", "12345678" );
        j.set( "name", "Test" );

        jpp::object jstate = jpp::make_object();

        for( const HashtableDevToDebugTabs::value_type & value : m_tabs )
        {
            const ConstString & key = value.key;
            const DevToDebugTabInterfacePtr & tab = value.element;

            jpp::array jtab = jpp::make_array();

            tab->foreachWidgets( [&jtab]( const DevToDebugWidgetInterfacePtr & _widget )
            {
                DevToDebugWidgetPtr widget = DevToDebugWidgetPtr::dynamic_from( _widget );

                jpp::object jwidget = jpp::make_object();

                widget->fillJson( jwidget );

                jtab.push_back( jwidget );
            } );

            jstate.set( key, jtab );
        }

        j.set( "state", jstate );

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object DevToDebugService::makeProcessData()
    {
        jpp::object j = jpp::make_object();

        j.set( "confirmed_revision", m_revision );

        return j;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperRunComplete_()
    {
        this->process();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperFinalizeGame_()
    {
        if( m_uuid.empty() == true )
        {
            return;
        }

        Char connect_url[256] = {'\0'};
        MENGINE_SPRINTF( connect_url, "http://www.devtodebug.com/api/worker/%s/delete/"
            , m_uuid.c_str()
        );

        cURLHeaders headers;
        headers.push_back( "Content-Type:application/json" );

        jpp::object j = jpp::make_object();

        String data;
        Helper::writeJSONString( j, &data );

        CURL_SERVICE()
            ->headerData( connect_url, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}