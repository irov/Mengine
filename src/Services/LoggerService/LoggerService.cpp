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
#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_LOGGER_LEVEL_FORCE_VERBOSE_ENABLE)
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

        const Char * loggerLevels[] = {
            "SILENT",               // LM_SILENT
            "FATAL",                // LM_FATAL
            "MESSAGE_RELEASE",      // LM_MESSAGE_RELEASE
            "ERROR",                // LM_ERROR  
            "WARNING",              // LM_WARNING
            "MESSAGE",              // LM_MESSAGE
            "INFO",                 // LM_INFO
            "DEBUG",                // LM_DEBUG
            "VERBOSE"               // LM_VERBOSE
        };

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
        msg.category = MENGINE_CODE_LIBRARY;
        msg.dateTime = dateTime;
        msg.level = LM_MESSAGE;
        msg.flag = LFLAG_SHORT;
        msg.filter = 0;
        msg.color = LCOLOR_GREEN;
        msg.file = "";
        msg.line = 0;
        msg.data = loggerLevelMessage;
        msg.size = loggerLevelMessageLen;

        this->logHistory_( msg );

        ThreadMutexInterfacePtr mutexLogger = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexLogger );

        m_mutexLogger = mutexLogger;

        ThreadMutexInterfacePtr mutexHistory = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexHistory );

        m_mutexHistory = mutexHistory;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CONFIGS_LOAD, &LoggerService::notifyConfigsLoad_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CONFIGS_LOAD );

        m_mutexLogger = nullptr;
        m_mutexHistory = nullptr;

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

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexHistory );
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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexHistory );
        m_history.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::hasVerbose( const Char * _category ) const
    {
        for( const ConstString & verbose : m_verboses )
        {
            if( verbose != _category )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::validMessage( const Char * _category, ELoggerLevel _level, uint32_t _filter ) const
    {
        MENGINE_UNUSED( _category );

        if( m_silent == true )
        {
            return false;
        }

        if( m_verboseLevel < _level )
        {
            if( this->hasVerbose( _category ) == false )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _message.category, "please setup category for log message ");
        MENGINE_ASSERTION_MEMORY_PANIC( _message.data, "please setup data for log message " );

        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_BEGIN, _message );

        this->logMessage_( _message );
        this->logHistory_( _message );

        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_END, _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage_( const LoggerMessage & _message )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        ++m_countMessage[_message.level];

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            if( logger->validMessage( _message ) == false )
            {
                continue;
            }

            logger->log( _message );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logHistory_( const LoggerMessage & _message )
    {
        if( m_historically == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexHistory );

        if( m_historyLimit != MENGINE_UNKNOWN_SIZE && m_history.size() >= m_historyLimit )
        {
            return;
        }

        HistoryRecord history;
        history.category = _message.category;
        history.dateTime = _message.dateTime;
        history.level = _message.level;
        history.flag = _message.flag;
        history.filter = _message.filter;
        history.color = _message.color;
        history.file = _message.file;
        history.line = _message.line;
        history.data.assign( _message.data, _message.data + _message.size );

        m_history.emplace_back( history );
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
    uint32_t LoggerService::getCountMessage( ELoggerLevel _level ) const
    {
        uint32_t count = m_countMessage[_level];

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::lockMessage()
    {
        if( m_mutexLogger == nullptr )
        {
            return;
        }

        m_mutexLogger->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unlockMessage()
    {
        if( m_mutexLogger == nullptr )
        {
            return;
        }

        m_mutexLogger->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::writeHistory( const LoggerInterfacePtr & _logger ) const
    {
        MENGINE_UNUSED( _logger );

        if( m_historically == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexHistory );

        for( const HistoryRecord & record : m_history )
        {
            LoggerMessage msg;
            msg.category = record.category;
            msg.dateTime = record.dateTime;
            msg.level = record.level;
            msg.flag = record.flag;
            msg.filter = record.filter;
            msg.color = record.color;
            msg.file = record.file;
            msg.line = record.line;
            msg.data = record.data.data();
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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

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
