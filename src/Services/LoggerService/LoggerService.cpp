#include "LoggerService.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"

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

#if MENGINE_LOGGER_LEVEL_FORCE_VERBOSE == 1
#   define MENGINE_LOGGER_LEVEL_FORCE_VERBOSE_ENABLE
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
        , m_silentMessageRelease( false )
        , m_historically( true )
        , m_historyLimit( MENGINE_LOGGER_HISTORY_MAX )
    {
        Algorithm::fill_n( m_countMessage, LOGGER_LEVEL_COUNT, 0u );
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

        bool OPTION_nomessagerelease = HAS_OPTION( "nomessagerelease" );

        if( OPTION_nomessagerelease == true )
        {
            m_silentMessageRelease = true;
        }

        ELoggerLevel logLevel;

        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roaming = HAS_OPTION( "roaming" );
        bool OPTION_noroaming = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (OPTION_roaming == false || OPTION_noroaming == true) )
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

        if( TEST_OPTION_VALUE( "log", "info" ) == true )
        {
            logLevel = LM_INFO;
        }
        else if( TEST_OPTION_VALUE( "log", "message" ) == true )
        {
            logLevel = LM_MESSAGE;
        }
        else if( TEST_OPTION_VALUE( "log", "warning" ) == true )
        {
            logLevel = LM_WARNING;
        }
        else if( TEST_OPTION_VALUE( "log", "error" ) == true )
        {
            logLevel = LM_ERROR;
        }
        else if( TEST_OPTION_VALUE( "log", "fatal" ) == true )
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

#ifdef MENGINE_LOGGER_LEVEL_FORCE_VERBOSE_ENABLE
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
        int32_t loggerLevelMessageLen = MENGINE_SNPRINTF( loggerLevelMessage, 255, "start logger with verbose level [%s] Mode [%s] Debug [%s] Master [%s] Publish [%s]"
            , loggerLevel
            , developmentMode == true ? "Dev" : "Normal"
            , MENGINE_DEBUG_VALUE( "True", "False" )
            , MENGINE_MASTER_RELEASE_VALUE( "True", "False" )
            , MENGINE_BUILD_PUBLISH_VALUE( "True", "False" )
        );

        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LoggerMessage msg;
        msg.category = ConstString::none();
        msg.dateTime = dateTime;
        msg.level = LM_MESSAGE;
        msg.filter = 0;
        msg.color = LCOLOR_GREEN;
        msg.data = loggerLevelMessage;
        msg.size = loggerLevelMessageLen;

        this->logHistory_( msg );

        ThreadMutexInterfacePtr threadMutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( threadMutex );

        m_mutex = threadMutex;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CONFIGS_LOAD, &LoggerService::notifyConfigsLoad_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CONFIGS_LOAD );

        m_mutex = nullptr;

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
    void LoggerService::setSilentMessageRelease( bool _silentMessageRelease )
    {
        m_silentMessageRelease = _silentMessageRelease;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::isSilentMessageRelease() const
    {
        return m_silentMessageRelease;
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
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        size_t size = Helper::makeLoggerTimestamp( dateTime, "[%02u:%02u:%02u:%04u] ", _buffer + _offset, _capacity - _offset );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter ) const
    {
        MENGINE_UNUSED( _category );

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

        if( m_silentMessageRelease == true )
        {
            if( _level == LM_MESSAGE_RELEASE )
            {
                return false;
            }
        }

        if( _filter != 0 )
        {
            if( (m_verboseFilter & _filter) == 0 )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage( const LoggerMessage & _message )
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_BEGIN, _message );

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

            ++m_countMessage[_message.level];

            for( const LoggerInterfacePtr & logger : m_loggers )
            {
                if( logger->validMessage( _message ) == false )
                {
                    continue;
                }

                logger->log( _message );
            }

            this->logHistory_( _message );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_END, _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logHistory_( const LoggerMessage & _message )
    {
        if( m_historically == true )
        {
            if( m_historyLimit != MENGINE_UNKNOWN_SIZE && m_history.size() >= m_historyLimit )
            {
                return;
            }

            Record history;
            history.category = _message.category;
            history.dateTime = _message.dateTime;
            history.level = _message.level;
            history.filter = _message.filter;
            history.color = _message.color;
            history.data.assign( _message.data, _message.size );

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
    void LoggerService::lockMessage()
    {
        if( m_mutex == nullptr )
        {
            return;
        }

        m_mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unlockMessage()
    {
        if( m_mutex == nullptr )
        {
            return;
        }

        m_mutex->unlock();
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
            LoggerMessage msg;
            msg.category = record.category;
            msg.dateTime = record.dateTime;
            msg.level = record.level;
            msg.filter = record.filter;
            msg.color = record.color;
            msg.data = record.data.c_str();
            msg.size = record.data.size();

            if( _logger->validMessage( msg ) == false )
            {
                continue;
            }

            _logger->log( msg );
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
