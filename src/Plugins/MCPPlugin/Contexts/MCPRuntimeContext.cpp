#include "MCPRuntimeContext.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const size_t MCP_RUNTIME_LOG_LIMIT = 5000;
    //////////////////////////////////////////////////////////////////////////
    MCPRuntimeContext::MCPRuntimeContext()
        : m_nextLogSequence( 1 )
        , m_paused( false )
        , m_advancing( false )
        , m_frozen( false )
        , m_timeScaled( false )
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
        log.timestamp = _message.timestamp;
        log.level = _message.level;
        log.category = _message.category != nullptr ? _message.category : "";
        log.message.assign( _message.data, _message.size );

        MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );

        log.sequence = m_nextLogSequence++;

        m_logs.emplace_back( StdUtility::move( log ) );

        while( m_logs.size() > MCP_RUNTIME_LOG_LIMIT )
        {
            m_logs.pop_front();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::restoreRuntime()
    {
        if( m_advancing == true )
        {
            this->endAdvance();
        }

        m_paused = false;

        this->updateFreeze_();

        if( m_timeScaled == false )
        {
            return;
        }

        m_timeScaled = false;

        TIMELINE_SERVICE()
            ->setTimeFactorBase( m_previousTimeFactorBase );

        TIMELINE_SERVICE()
            ->setTimeFactorCount( m_previousTimeFactorCount );
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & MCPRuntimeContext::getLogMutex() const
    {
        return m_logMutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const MCPRuntimeContext::DequeRuntimeLogs & MCPRuntimeContext::getLogs() const
    {
        return m_logs;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::setPaused( bool _paused )
    {
        m_paused = _paused;

        this->updateFreeze_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPRuntimeContext::isPaused() const
    {
        return m_paused;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::setTimeScale( float _timeScale )
    {
        if( m_timeScaled == false )
        {
            m_timeScaled = true;

            m_previousTimeFactorBase = TIMELINE_SERVICE()
                ->getTimeFactorBase();

            m_previousTimeFactorCount = TIMELINE_SERVICE()
                ->getTimeFactorCount();
        }

        TIMELINE_SERVICE()
            ->setTimeFactorCount( 0 );

        TIMELINE_SERVICE()
            ->setTimeFactorBase( _timeScale );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::beginAdvance()
    {
        m_advancing = true;

        this->updateFreeze_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::endAdvance()
    {
        m_advancing = false;

        this->updateFreeze_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPRuntimeContext::isAdvancing() const
    {
        return m_advancing;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPRuntimeContext::updateFreeze_()
    {
        bool frozen = m_paused == true && m_advancing == false;

        if( m_frozen == frozen )
        {
            return;
        }

        m_frozen = frozen;

        APPLICATION_SERVICE()
            ->setUpdateFreeze( STRINGIZE_STRING_LOCAL( "MCP" ), m_frozen );
    }
    //////////////////////////////////////////////////////////////////////////
}
