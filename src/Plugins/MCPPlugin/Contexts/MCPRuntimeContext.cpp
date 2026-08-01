#include "MCPRuntimeContext.h"

#include "Interface/TimelineServiceInterface.h"

#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPRuntimeContext::MCPRuntimeContext()
        : m_nextLogSequence( 1 )
        , m_paused( false )
        , m_previousTimeFactorBase( 1.f )
        , m_previousTimeFactorCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPRuntimeContext::initialize()
    {
        m_logMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        if( m_logMutex == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::finalize()
    {
        this->restoreRuntime();

        m_logs.clear();
        m_logMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::captureLog( const LoggerMessage & _message )
    {
        RuntimeLog log;
        log.sequence = m_nextLogSequence++;
        log.timestamp = _message.timestamp;
        log.level = _message.level;
        log.category = _message.category != nullptr ? _message.category : "";
        log.message.assign( _message.data, _message.size );

        MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );
        m_logs.emplace_back( StdUtility::move( log ) );

        static const size_t MCP_RUNTIME_LOG_LIMIT = 5000;

        while( m_logs.size() > MCP_RUNTIME_LOG_LIMIT )
        {
            m_logs.pop_front();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::restoreRuntime()
    {
        if( m_paused == false )
        {
            return;
        }

        TIMELINE_SERVICE()->setTimeFactorBase( m_previousTimeFactorBase );
        TIMELINE_SERVICE()->setTimeFactorCount( m_previousTimeFactorCount );
        m_paused = false;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & MCPRuntimeContext::getLogMutex() const
    {
        const ThreadMutexInterfacePtr & mutex = m_logMutex;

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const MCPRuntimeContext::DequeRuntimeLogs & MCPRuntimeContext::getLogs() const
    {
        const DequeRuntimeLogs & logs = m_logs;

        return logs;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPRuntimeContext::isPaused() const
    {
        bool paused = m_paused;

        return paused;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::setPaused( bool _paused )
    {
        m_paused = _paused;
    }
    //////////////////////////////////////////////////////////////////////////
    float MCPRuntimeContext::getPreviousTimeFactorBase() const
    {
        float value = m_previousTimeFactorBase;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::setPreviousTimeFactorBase( float _value )
    {
        m_previousTimeFactorBase = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MCPRuntimeContext::getPreviousTimeFactorCount() const
    {
        int32_t value = m_previousTimeFactorCount;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::setPreviousTimeFactorCount( int32_t _value )
    {
        m_previousTimeFactorCount = _value;
    }
    //////////////////////////////////////////////////////////////////////////
}
