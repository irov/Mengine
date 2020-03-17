#include "LoggerService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/DocumentHelper.h"

#include "Kernel/String.h"
#include "Kernel/StringRegex.h"

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
#ifdef MENGINE_LOGGER_HISTORY
        , m_historically( true )
#endif
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

        ELoggerLevel logLevel;

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (roamingMode == false || noroamingMode == true) )
        {
            logLevel = LM_MESSAGE;
        }
        else
        {
#ifdef MENGINE_DEBUG
            logLevel = LM_MESSAGE;
#else
            logLevel = LM_ERROR;
#endif
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
            this->setVerboseLevel( LM_MAX );
        }

        SERVICE_WAIT( Mengine::ThreadServiceInterface, [this]()
        {
            ThreadMutexInterfacePtr threadMutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( threadMutex, false );

            m_threadMutex = threadMutex;

            return true;
        } );

        SERVICE_LEAVE( Mengine::ThreadServiceInterface, [this]()
        {
            m_threadMutex = nullptr;
        } );

        SERVICE_WAIT( Mengine::PlatformInterface, [this]()
        {
            DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
                ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider, false );

            m_dateTimeProvider = dateTimeProvider;

            return true;
        } );

        SERVICE_LEAVE( Mengine::PlatformInterface, [this]()
        {
            m_dateTimeProvider = nullptr;
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
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_stopService()
    {
#ifdef MENGINE_LOGGER_HISTORY
        m_historically = false;
        m_history.clear();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::setVerboseLevel( ELoggerLevel _level )
    {
        m_verboseLevel = _level;
    }
    //////////////////////////////////////////////////////////////////////////
    ELoggerLevel LoggerService::getVerboseLevel() const
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
    int32_t LoggerService::makeTimeStamp( Char * _buffer, int32_t _offset, size_t _capacity ) const
    {
        if( m_dateTimeProvider == nullptr )
        {
            return 0;
        }

        PlatformDateTime dateTime;
        m_dateTimeProvider->getLocalDateTime( &dateTime );

        int size = snprintf( _buffer + _offset, _capacity - _offset, "[%02u:%02u:%02u:%04u] "
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
            , dateTime.milliseconds
        );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t LoggerService::makeFunctionStamp( const Char * _file, uint32_t _line, Char * _buffer, int32_t _offset, size_t _capacity ) const
    {
        if( _file == nullptr )
        {
            return 0;
        }

        String str_function = _file;

        StringRegex regex_lambda_remove( "::<lambda_.*>::operator \\(\\)" );

        StringMatchResults match_lambda_remove;
        while( std::regex_search( str_function, match_lambda_remove, regex_lambda_remove ) == true )
        {
            const std::sub_match<String::const_iterator> & lambda_remove_prefix = match_lambda_remove.prefix();
            const std::sub_match<String::const_iterator> & lambda_remove_suffix = match_lambda_remove.suffix();

            str_function = String( lambda_remove_prefix.first, lambda_remove_prefix.second ) + String( lambda_remove_suffix.first, lambda_remove_suffix.second );
        }

        StringRegex regex_engine_remove( "Mengine::" );

        StringMatchResults match_engine_remove;
        if( std::regex_search( str_function, match_engine_remove, regex_engine_remove ) == true )
        {
            const std::sub_match<String::const_iterator> & engine_remove_prefix = match_engine_remove.prefix();
            const std::sub_match<String::const_iterator> & engine_remove_suffix = match_engine_remove.suffix();

            str_function = String( engine_remove_prefix.first, engine_remove_prefix.second ) + String( engine_remove_suffix.first, engine_remove_suffix.second );
        }

        int32_t size = snprintf( _buffer + _offset, _capacity - _offset, "%s[%u] "
            , str_function.c_str()
            , _line
        );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( ELoggerLevel _level, uint32_t _flag ) const
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
    void LoggerService::logMessage( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _message, size_t _size )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_threadMutex );

        ++m_countMessage[_level];

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            if( logger->validMessage( _level, _flag ) == false )
            {
                continue;
            }

            logger->log( _level, _flag, _color, _message, _size );
        }

#ifdef MENGINE_LOGGER_HISTORY
        if( m_historically == true )
        {
            Record history;
            history.level = _level;
            history.flag = _flag;
            history.color = _color;
            history.message.assign( _message, _size );

            m_history.emplace_back( history );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerService::getCountMessage( ELoggerLevel _level )
    {
        uint32_t count = m_countMessage[_level];

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::writeHistory( const LoggerInterfacePtr & _logger ) const
    {
        MENGINE_UNUSED( _logger );

#ifdef MENGINE_LOGGER_HISTORY
        MENGINE_THREAD_MUTEX_SCOPE( m_threadMutex );

        for( const Record & record : m_history )
        {
            const Char * record_message_str = record.message.c_str();
            String::size_type record_message_size = record.message.size();

            _logger->log( record.level, record.flag, record.color, record_message_str, record_message_size );
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
