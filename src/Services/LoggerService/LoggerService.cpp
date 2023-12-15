#include "LoggerService.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadSharedMutexScope.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/String.h"
#include "Kernel/StringRegex.h"
#include "Kernel/BuildMode.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/StatisticHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"
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

#ifndef MENGINE_LOGGER_MESSAGE_BUFFER_MAX
#define MENGINE_LOGGER_MESSAGE_BUFFER_MAX 64
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
        m_staticsticLevel[LM_SILENT] = STATISTIC_LOGGER_MESSAGE_SILENT;
        m_staticsticLevel[LM_FATAL] = STATISTIC_LOGGER_MESSAGE_FATAL;
        m_staticsticLevel[LM_MESSAGE_RELEASE] = STATISTIC_LOGGER_MESSAGE_MESSAGE_RELEASE;
        m_staticsticLevel[LM_ERROR] = STATISTIC_LOGGER_MESSAGE_ERROR;
        m_staticsticLevel[LM_WARNING] = STATISTIC_LOGGER_MESSAGE_WARNING;
        m_staticsticLevel[LM_MESSAGE] = STATISTIC_LOGGER_MESSAGE_MESSAGE;
        m_staticsticLevel[LM_INFO] = STATISTIC_LOGGER_MESSAGE_INFO;
        m_staticsticLevel[LM_DEBUG] = STATISTIC_LOGGER_MESSAGE_DEBUG;
        m_staticsticLevel[LM_VERBOSE] = STATISTIC_LOGGER_MESSAGE_VERBOSE;
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

        ELoggerLevel logLevel = LM_MESSAGE;

        bool developmentMode = Helper::isDevelopmentMode(); 

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

        m_messages.reserve( MENGINE_LOGGER_MESSAGE_BUFFER_MAX );
        m_messagesAux.reserve( MENGINE_LOGGER_MESSAGE_BUFFER_MAX );

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

        ThreadSharedMutexInterfacePtr mutexMessage = THREAD_SYSTEM()
            ->createSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexMessage );

        m_mutexMessage = mutexMessage;        

        ThreadSharedMutexInterfacePtr mutexLogger = THREAD_SYSTEM()
            ->createSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexLogger );

        m_mutexLogger = mutexLogger;

        ThreadSharedMutexInterfacePtr mutexHistory = THREAD_SYSTEM()
            ->createSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexHistory );

        m_mutexHistory = mutexHistory;

        ThreadMutexInterfacePtr mutexMessageBlock = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexMessageBlock );

        m_mutexMessageBlock = mutexMessageBlock;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CONFIGS_LOAD, &LoggerService::notifyConfigsLoad_, MENGINE_DOCUMENT_FACTORABLE );

        ThreadConditionVariableInterfacePtr conditionLogger = THREAD_SYSTEM()
            ->createConditionVariable( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( conditionLogger );

        m_conditionLogger = conditionLogger;

        ThreadIdentityInterfacePtr threadLogger = THREAD_SYSTEM()
            ->createThreadIdentity( STRINGIZE_STRING_LOCAL( "LoggerService" ), ETP_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE );

        ThreadIdentityRunnerInterfacePtr threadRunner = threadLogger->run( [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            this->processMessages_( _runner );
        } );

        m_threadLogger = threadLogger;
        m_threadRunner = threadRunner;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CONFIGS_LOAD );

        m_memoryOldLog = nullptr;

        m_mutexMessage = nullptr;
        m_mutexHistory = nullptr;
        m_mutexMessageBlock = nullptr;
        m_mutexLogger = nullptr;

        m_loggers.clear();
        m_verboses.clear();
        m_history.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_stopService()
    {
        m_historically = false;

        m_mutexHistory->lock();
        m_history.clear();
        m_mutexHistory->unlock();

        m_mutexLogger->lock();

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            logger->flush();
        }

        m_mutexLogger->unlock();

        m_threadLogger->cancel();

        m_threadRunner = nullptr;
        m_threadLogger = nullptr;
        m_conditionLogger = nullptr;
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
        MENGINE_ASSERTION_MEMORY_PANIC( _message.category, "please setup category for log message" );
        MENGINE_ASSERTION_MEMORY_PANIC( _message.data, "please setup data for log message" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_BEGIN, _message );

        this->logMessage_( _message );
        this->logHistory_( _message );

        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_END, _message );

        if( m_conditionLogger != nullptr )
        {
            m_conditionLogger->wake();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage_( const LoggerMessage & _message )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexMessageBlock );

        ELoggerLevel level = _message.level;

        uint32_t statisticId = m_staticsticLevel[level];

        STATISTIC_INC_INTEGER( statisticId );

        LoggerMessageRecord record;
        MENGINE_STRNCPY( record.category, _message.category, sizeof( record.category ) );
        record.dateTime = _message.dateTime;
        record.level = _message.level;
        record.flag = _message.flag;
        record.filter = _message.filter;
        record.color = _message.color;
        record.file = _message.file;
        record.line = _message.line;
        MENGINE_MEMCPY( record.data, _message.data, _message.size );
        record.data[_message.size] = '\0';
        record.size = _message.size;

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexMessage );

        if( m_messages.size() >= MENGINE_LOGGER_MESSAGE_BUFFER_MAX )
        {
            return;
        }

        m_messages.emplace_back( record );
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
    void LoggerService::processMessages_( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        for( ;; )
        {
            m_conditionLogger->wait( [this, _runner]()
            {
                if( _runner->isCancel() == true )
                {
                    return true;
                }

                if( m_messages.empty() == true )
                {
                    return false;
                }

                return true;
            } );

            if( _runner->isCancel() == true )
            {
                break;
            }

            m_mutexMessage->lock();
            std::swap( m_messagesAux, m_messages );
            m_messages.clear();
            m_mutexMessage->unlock();

            MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutexLogger );

            for( const LoggerInterfacePtr & logger : m_loggers )
            {
                for( const LoggerMessageRecord & record : m_messagesAux )
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
                    msg.data = record.data;
                    msg.size = MENGINE_STRLEN( record.data );

                    if( logger->validMessage( msg ) == false )
                    {
                        continue;
                    }

                    logger->log( msg );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::lockMessage()
    {
        if( m_mutexMessageBlock == nullptr )
        {
            return;
        }

        m_mutexMessageBlock->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unlockMessage()
    {
        if( m_mutexMessageBlock == nullptr )
        {
            return;
        }

        m_mutexMessageBlock->unlock();
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
    bool LoggerService::loadOldLogMemory( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        if( _fileGroup->existFile( _filePath, false ) == false )
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryFileString( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr )
        {
            return false;
        }

        m_memoryOldLog = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & LoggerService::getOldLogMemory() const
    {
        return m_memoryOldLog;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::registerLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_ASSERTION_FATAL( Algorithm::find( m_loggers.begin(), m_loggers.end(), _logger ) == m_loggers.end(), "already register logger" );

        if( _logger->initializeLogger() == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        m_loggers.emplace_back( _logger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unregisterLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_ASSERTION_FATAL( Algorithm::find( m_loggers.begin(), m_loggers.end(), _logger ) != m_loggers.end(), "not found logger" );

        _logger->flush();

        _logger->finalizeLogger();

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        VectorLoggers::iterator it_find =
            Algorithm::find( m_loggers.begin(), m_loggers.end(), _logger );

        m_loggers.erase( it_find );
    }
    //////////////////////////////////////////////////////////////////////////
}
