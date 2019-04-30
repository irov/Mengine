#include "LoggerService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Document.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoggerService, Mengine::LoggerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerService::LoggerService()
        : m_verboseLevel( LM_ERROR )
        , m_verboseFlag( 0 )
        , m_silent( false )
    {
        for( uint32_t i = 0; i != LM_MAX; ++i )
        {
            m_countMessage[i] = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerService::~LoggerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::_initializeService()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            m_silent = true;
        }

        EMessageLevel logLevel;

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (roamingMode == false || noroamingMode == true) )
        {
            logLevel = LM_MESSAGE;
        }
        else
        {
            logLevel = LM_ERROR;
        }

        if( TEST_OPTION_VALUE( "log", "0" ) == true )
        {
            logLevel = LM_INFO;
        }
        else if( TEST_OPTION_VALUE( "log", "1" ) == true )
        {
            logLevel = LM_MESSAGE;
        }
        else if( TEST_OPTION_VALUE( "log", "2" ) == true )
        {
            logLevel = LM_WARNING;
        }
        else if( TEST_OPTION_VALUE( "log", "3" ) == true )
        {
            logLevel = LM_ERROR;
        }
        else if( TEST_OPTION_VALUE( "log", "4" ) == true )
        {
            logLevel = LM_CRITICAL;
        }
        else if( TEST_OPTION_VALUE( "log", "5" ) == true )
        {
            logLevel = LM_FATAL;
        }

        this->setVerboseLevel( logLevel );

        uint32_t verboseFlag = 0;

        bool profiler = HAS_OPTION( "profiler" );

        if( profiler == true )
        {
            verboseFlag |= 0x00000001;
        }

        this->setVerboseFlag( verboseFlag );

        if( HAS_OPTION( "verbose" ) == true )
        {
            LOGGER_SERVICE()
                ->setVerboseLevel( LM_MAX );

            LOGGER_INFO( "Verbose logging mode enabled" );
        }

        SERVICE_WAIT( Mengine::ThreadServiceInterface, [this]()
        {
            m_threadMutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FUNCTION );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            logger->flush();
        }

        m_loggers.clear();

#ifdef MENGINE_LOGGER_HISTORY
        m_history.clear();
#endif

        m_threadMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setVerboseLevel( EMessageLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    EMessageLevel LoggerService::getVerboseLevel() const
    {
        return m_verboseLevel;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerService::getVerboseFlag() const
    {
        return m_verboseFlag;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setSilent( bool _silent )
    {
        m_silent = _silent;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::isSilent() const
    {
        return m_silent;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( EMessageLevel _level, uint32_t _flag ) const
    {
        if( m_silent == true )
        {
            return false;
        }

        if( m_verboseLevel < _level )
        {
            return false;
        }

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage( EMessageLevel _level, uint32_t _flag, const Char * _message, uint32_t _size )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_threadMutex );

        ++m_countMessage[_level];

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            if( logger->validMessage( _level, _flag ) == false )
            {
                continue;
            }

            logger->log( _level, _flag, _message, _size );
        }

#ifdef MENGINE_LOGGER_HISTORY
        Record history;
        history.level = _level;
        history.flag = _flag;
        history.message.assign( _message, _size );

        m_history.emplace_back( history );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerService::getCountMessage( EMessageLevel _level )
    {
        uint32_t count = m_countMessage[_level];

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::writeHistory( const LoggerInterfacePtr & _logger ) const
    {
        MENGINE_UNUSED( _logger );

#ifdef MENGINE_LOGGER_HISTORY
        for( const Record & record : m_history )
        {
            const Char * record_message_str = record.message.c_str();
            String::size_type record_message_size = record.message.size();

            _logger->log( record.level, record.flag, record_message_str, (uint32_t)record_message_size );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::registerLogger( const LoggerInterfacePtr & _logger )
    {
        VectorLoggers::iterator it_find =
            std::find( m_loggers.begin(), m_loggers.end(), _logger );

        if( it_find != m_loggers.end() )
        {
            return false;
        }

        if( _logger->initialize() == false )
        {
            return false;
        }

        m_loggers.emplace_back( _logger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::unregisterLogger( const LoggerInterfacePtr & _logger )
    {
        VectorLoggers::iterator it_find =
            std::find( m_loggers.begin(), m_loggers.end(), _logger );

        if( it_find == m_loggers.end() )
        {
            return false;
        }

        _logger->flush();

        _logger->finalize();

        m_loggers.erase( it_find );

        return true;
    }
}
