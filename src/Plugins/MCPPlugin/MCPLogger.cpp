#include "MCPLogger.h"

#include "MCPService.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPLogger::MCPLogger()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPLogger::~MCPLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPLogger::setService( MCPService * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        if( m_service == nullptr )
        {
            return;
        }

        LoggerMessage message;
        _record->getMessage( &message );
        m_service->captureRuntimeLog( message );
    }
    //////////////////////////////////////////////////////////////////////////
}
