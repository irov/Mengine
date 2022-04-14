#include "LoggerService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/String.h"
#include "Kernel/StringRegex.h"
#include "Kernel/BuildMode.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdIO.h"

#include "Config/Algorithm.h"

#ifndef MENGINE_LOGGER_LEVEL_FORCE_VERBOSE
#define MENGINE_LOGGER_LEVEL_FORCE_VERBOSE 0
#endif

#ifndef MENGINE_LOGGER_HISTORY_MAX
#define MENGINE_LOGGER_HISTORY_MAX 1048576
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoggerService, Mengine::LoggerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerService::LoggerService()
        : m_verboseLevel( LM_ERROR )
        , m_verboseFilter( 0xFFFFFFFF )
        , m_silent( false )
        , m_historically( true )
        , m_historyLimit( MENGINE_LOGGER_HISTORY_MAX )
    {
        Algorithm::fill_n( m_countMessage, __LM_MAX__, 0u );
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerService::~LoggerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::_initializeService()
    {
        bool OPTION_nologs = HAS_OPTION( "nologs" );

        if( OPTION_nologs == true )
        {
            m_silent = true;
        }

        ELoggerLevel logLevel;

        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roamingMode = HAS_OPTION( "roaming" );
        bool OPTION_noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (OPTION_roamingMode == false || OPTION_noroamingMode == true) )
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
        else if( HAS_OPTION( "silent" ) == true )
        {
            logLevel = LM_SILENT;
        }
        else if( HAS_OPTION( "verbose" ) == true )
        {
            logLevel = LM_VERBOSE;
        }

#if MENGINE_LOGGER_LEVEL_FORCE_VERBOSE == 1
        logLevel = LM_VERBOSE;
#endif

        this->setVerboseLevel( logLevel );

        uint32_t verboseFilter = 0xFFFFFFFF;

        this->setVerboseFilter( verboseFilter );

        const Char * verboses[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t verboses_count;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "verboses", verboses, &verboses_count ) == true )
        {
            for( uint32_t index = 0; index != verboses_count; ++index )
            {
                const Char * verbose = verboses[index];

                m_verboses.emplace_back( Helper::stringizeString( verbose ) );
            }
        }

        if( HAS_OPTION( "loghistory" ) == true )
        {
            m_historically = true;
        }
        else if( HAS_OPTION( "nologhistory" ) == true )
        {
            m_historically = false;
        }

        ELoggerLevel level = this->getVerboseLevel();
        const Char * loggerLevels[] = {"SILENT", "FATAL", "CRITICAL", "MESSAGE_RELEASE", "ERROR", "PERFOMANCE", "STATISTIC", "WARNING", "MESSAGE", "INFO", "DEBUG", "VERBOSE"};

        const Char * loggerLevel = loggerLevels[level];

        Char loggerLevelMessage[256] = {'\0'};
        int32_t loggerLevelMessageLen = MENGINE_SNPRINTF( loggerLevelMessage, 255, "start logger with verbose level [%s] Mode [%s] Debug [%s] Master [%s] Publish [%s]\n"
            , loggerLevel
            , developmentMode == true ? "Dev" : "Normal"
            , MENGINE_DEBUG_VALUE( "True", "False" )
            , MENGINE_MASTER_RELEASE_VALUE( "True", "False" )
            , MENGINE_BUILD_PUBLISH_VALUE( "True", "False" )
        );

        this->logHistory_( LM_MESSAGE, 0, LCOLOR_GREEN, loggerLevelMessage, loggerLevelMessageLen );

        SERVICE_WAIT( Mengine::ThreadServiceInterface, [this]()
        {
            ThreadMutexInterfacePtr threadMutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( threadMutex );

            m_mutex = threadMutex;

            return true;
        } );

        SERVICE_LEAVE( Mengine::ThreadServiceInterface, [this]()
        {
            m_mutex = nullptr;
        } );

        SERVICE_WAIT( Mengine::PlatformInterface, [this]()
        {
            DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
                ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider );

            m_dateTimeProvider = dateTimeProvider;

            return true;
        } );

        SERVICE_LEAVE( Mengine::PlatformInterface, [this]()
        {
            m_dateTimeProvider = nullptr;
        } );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CONFIGS_LOAD, &LoggerService::notifyConfigsLoad_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CONFIGS_LOAD );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            logger->flush();
        }

        m_loggers.clear();
        m_verboses.clear();
        m_history.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_stopService()
    {
        m_historically = false;
        m_history.clear();
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
    void LoggerService::setVerboseFilter( uint32_t _filter )
    {
        m_verboseFilter = _filter;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t LoggerService::getVerboseFilter() const
    {
        return m_verboseFilter;
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
    void LoggerService::setHistorically( bool _historically )
    {
        m_historically = _historically;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::getHistorically() const
    {
        return m_historically;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::clearHistory()
    {
        m_history.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t LoggerService::makeTimeStamp( Char * const _buffer, size_t _offset, size_t _capacity ) const
    {
        if( m_dateTimeProvider == nullptr )
        {
            return 0;
        }

        size_t size = Helper::makeLoggerTimestamp( m_dateTimeProvider, "[%02u:%02u:%02u:%04u] ", _buffer + _offset, _capacity - _offset );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t LoggerService::makeFunctionStamp( const Char * _file, uint32_t _line, Char * const _buffer, size_t _offset, size_t _capacity ) const
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

        int32_t size = MENGINE_SNPRINTF( _buffer + _offset, _capacity - _offset, "%s[%u] "
            , str_function.c_str()
            , _line
        );

        return (size_t)size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter ) const
    {
        if( m_silent == true )
        {
            return false;
        }

        if( m_verboseLevel < _level )
        {
            if( _category == ConstString::none() )
            {
                return false;
            }

            if( Algorithm::find( m_verboses.begin(), m_verboses.end(), _category ) == m_verboses.end() )
            {
                return false;
            }
        }

        if( _filter == 0 )
        {
            return true;
        }

        if( (m_verboseFilter & _filter) == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size )
    {
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_BEGIN, _level, _filter, _color, _message, _size );
        }

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

            ++m_countMessage[_level];

            for( const LoggerInterfacePtr & logger : m_loggers )
            {
                if( logger->validMessage( _level, _filter ) == false )
                {
                    continue;
                }

                logger->log( _level, _filter, _color, _message, _size );
            }

            this->logHistory_( _level, _filter, _color, _message, _size );
        }

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_END, _level, _filter, _color, _message, _size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logHistory_( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _message, size_t _size )
    {
        if( m_historically == true )
        {
            if( m_historyLimit != ~0U && m_history.size() >= m_historyLimit )
            {
                return;
            }

            Record history;
            history.level = _level;
            history.filter = _filter;
            history.color = _color;
            history.message.assign( _message, _size );

            m_history.emplace_back( history );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::notifyConfigsLoad_()
    {
        m_historyLimit = CONFIG_VALUE( "Limit", "LoggerHistoryMax", MENGINE_LOGGER_HISTORY_MAX );

        VectorConstString verboses;
        CONFIG_VALUES( "Engine", "Verboses", &verboses );

        m_verboses.insert( m_verboses.begin(), verboses.begin(), verboses.end() );
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

        if( m_historically == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const Record & record : m_history )
        {
            ELoggerLevel record_level = record.level;
            uint32_t record_filter = record.filter;

            if( _logger->validMessage( record_level, record_filter ) == false )
            {
                continue;
            }

            uint32_t record_color = record.color;
            const Char * record_message_str = record.message.c_str();
            String::size_type record_message_size = record.message.size();

            _logger->log( record_level, record_filter, record_color, record_message_str, record_message_size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::registerLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorLoggers::iterator it_find =
            Algorithm::find( m_loggers.begin(), m_loggers.end(), _logger );

        if( it_find != m_loggers.end() )
        {
            return false;
        }

        if( _logger->initializeLogger() == false )
        {
            return false;
        }

        m_loggers.emplace_back( _logger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::unregisterLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorLoggers::iterator it_find =
            Algorithm::find( m_loggers.begin(), m_loggers.end(), _logger );

        if( it_find == m_loggers.end() )
        {
            return false;
        }

        _logger->flush();

        _logger->finalizeLogger();

        m_loggers.erase( it_find );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
