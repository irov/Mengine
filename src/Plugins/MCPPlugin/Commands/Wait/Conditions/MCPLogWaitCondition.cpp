#include "MCPLogWaitCondition.h"

#include "../../../Contexts/MCPRuntimeContext.h"

#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPLogWaitCondition::MCPLogWaitCondition( MCPRuntimeContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPLogWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        const Char * contains = _request.condition.get( "contains", "" );
        const Char * category = _request.condition.get( "category", "" );
        String expectedCategory = category;
        int64_t afterValue = _request.condition.get( "after", MENGINE_INT64_C(0) );
        uint64_t after = afterValue > 0 ? (uint64_t)afterValue : 0;
        const ThreadMutexInterfacePtr & mutex = m_context->getLogMutex();

        MENGINE_THREAD_MUTEX_SCOPE( mutex );

        const MCPRuntimeContext::DequeRuntimeLogs & logs = m_context->getLogs();

        for( const MCPRuntimeContext::RuntimeLog & log : logs )
        {
            if( log.sequence <= after )
            {
                continue;
            }

            if( expectedCategory.empty() == false && expectedCategory != log.category )
            {
                continue;
            }

            const Char * message = log.message.c_str();

            if( contains[0] != '\0' && StdString::strstr( message, contains ) == nullptr )
            {
                continue;
            }

            _response->satisfied = true;
            _response->details.set( "sequence", (int64_t)log.sequence );
            _response->details.set( "category", log.category );
            _response->details.set( "message", log.message );

            return true;
        }

        _response->satisfied = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
