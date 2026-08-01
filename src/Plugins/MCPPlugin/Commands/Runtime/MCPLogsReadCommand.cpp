#include "MCPLogsReadCommand.h"

#include "../../Contexts/MCPRuntimeContext.h"

#include "Kernel/ThreadMutexScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPLogsReadCommand::MCPLogsReadCommand( MCPRuntimeContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPLogsReadCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        int64_t afterValue = _request.params.get( "after", MENGINE_INT64_C(0) );
        uint64_t after = afterValue > 0 ? (uint64_t)afterValue : 0;
        uint32_t limit = _request.params.get( "limit", MENGINE_UINT32_C(500) );

        if( limit > 5000 )
        {
            limit = 5000;
        }

        jpp::array records = jpp::make_array();
        uint64_t next = after;

        const ThreadMutexInterfacePtr & mutex = m_context->getLogMutex();

        {
            MENGINE_THREAD_MUTEX_SCOPE( mutex );

            const MCPRuntimeContext::DequeRuntimeLogs & logs = m_context->getLogs();

            for( const MCPRuntimeContext::RuntimeLog & log : logs )
            {
                if( log.sequence <= after )
                {
                    continue;
                }

                jpp::object record = jpp::make_object();
                record.set( "sequence", (int64_t)log.sequence );
                record.set( "timestamp", (int64_t)log.timestamp );
                record.set( "level", (uint32_t)log.level );
                record.set( "category", log.category );
                record.set( "message", log.message );
                records.push_back( record );
                next = log.sequence;

                if( records.size() >= limit )
                {
                    break;
                }
            }
        }

        _response->result.set( "records", records );
        _response->result.set( "next", (int64_t)next );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
