#include "LoggerBase.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadMutexHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::LoggerBase()
        : m_verboseLevel( LM_VERBOSE )
        , m_verboseFilter( 0xFFFFFFFF )
        , m_writeHistory( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerBase::~LoggerBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::initializeLogger()
    {
        if( this->_initializeLogger() == false )
        {
            return false;
        }

        if( m_writeHistory == true )
        {
            LOGGER_SERVICE()
                ->writeHistory( LoggerInterfacePtr::from( this ) );
        }

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::finalizeLogger()
    {
        this->_finalizeLogger();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setWriteHistory( bool _writeHistory )
    {
        m_writeHistory = _writeHistory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::getWriteHistory() const
    {
        return m_writeHistory;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseLevel( ELoggerLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    ELoggerLevel LoggerBase::getVerboseLevel() const
    {
        return m_verboseLevel;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::setVerboseFilter( uint32_t _filter )
    {
        m_verboseFilter = _filter;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerBase::getVerboseFilter() const
    {
        return m_verboseFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerBase::validMessage( const LoggerRecordInterfacePtr & _record ) const
    {
        LoggerMessage message;
        _record->getMessage( &message );

        if( m_verboseLevel < message.level )
        {
            return false;
        }

        if( message.filter == LFILTER_NONE )
        {
            return true;
        }

        if( (m_verboseFilter & message.filter) != message.filter )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::log( const LoggerRecordInterfacePtr & _record )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );
        
        this->_log( _record );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::flush()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        this->_flush();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerBase::_flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
