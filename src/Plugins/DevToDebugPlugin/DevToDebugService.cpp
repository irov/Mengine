#include "DevToDebugService.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"

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
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugService::~DevToDebugService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & DevToDebugService::requiredServices() const
    {
        static ServiceRequiredList required = {"cURLService"};

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

        this->process();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugService::_finalizeService()
    {
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
                cURLPostParams params;
                CURL_SERVICE()
                    ->postMessage( connect_url, headers, -1, 0, params, cURLReceiverInterfacePtr::from( this ) );                
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
}