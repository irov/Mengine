#include "DevToDebugService.h"

#include "DevToDebugWidget.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DevToDebugService, Mengine::DevToDebugService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{ 
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        static int jpp_dump_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
        {
            Char * state_str = (Char *)_ud;
            MENGINE_STRNCAT( state_str, _buffer, _size );

            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::DevToDebugService()
        : m_status( EDTDS_NONE )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );
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
                String data = R"({"did": "12345678", "name": "Test", "state": {"Dev": [ { "id": "btn1", "type": "button", "data": { "title": "Push Me!"}}]}})";
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
        MENGINE_UNUSED( _response );

        return;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::notifyBootstrapperRunComplete_()
    {
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

        Char state_str[65536] = {'\0'};
        jpp::jpp_bool_t result = jpp::dump_compact( jstate, &Helper::jpp_dump_callback, state_str );

        LOGGER_ERROR( "result: %d", result );
    }
    //////////////////////////////////////////////////////////////////////////
}