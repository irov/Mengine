#include "LoggerService.h"

#include "LoggerRecord.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionUtf8.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadSharedMutexScope.h"
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
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/FactoryPool.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"

#ifndef MENGINE_LOGGER_HISTORY_PREFETCH_SIZE
#define MENGINE_LOGGER_HISTORY_PREFETCH_SIZE 64
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoggerService, Mengine::LoggerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerService::LoggerService()
        : m_verboseLevel( MENGINE_RELEASE_VALUE( LM_WARNING, LM_MESSAGE ) )
        , m_verboseFilter( 0xFFFFFFFF )
        , m_silent( false )
        , m_silentMessageRelease( false )
        , m_historically( true )
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
        m_factoryLoggerRecord = Helper::makeFactoryPoolWithMutex<LoggerRecord, 32>( MENGINE_DOCUMENT_FACTORABLE );

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

        ELoggerLevel logLevel = m_verboseLevel;

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

        this->setVerboseLevel( logLevel );

        uint32_t verboseFilter = 0xFFFFFFFF;

        this->setVerboseFilter( verboseFilter );

        const Char * verboses[MENGINE_OPTIONS_VALUES_MAX] = {nullptr};
        uint32_t verboses_count = 0;
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

        if( m_historically == true )
        {
            m_history.reserve( MENGINE_LOGGER_HISTORY_PREFETCH_SIZE );
        }

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

        const Char * loggerLevel = loggerLevels[logLevel];
        
        const Char * buildVersion = Helper::getBuildVersion();
        const Char * buildNumber = Helper::getBuildNumberString();
        const Char * engineGitSHA1 = Helper::getEngineGitSHA1();
        bool developmentMode = Helper::isDevelopmentMode();

        Char loggerLevelMessage[512 + 1] = {'\0'};
        size_t loggerLevelMessageLen = MENGINE_SNPRINTF( loggerLevelMessage, 512, 
            "logger: %s [%s] [%s] [%s] [%s] %s [%s] %.8s"
            , loggerLevel
            , developmentMode == true ? "dev" : "prod"
            , MENGINE_DEBUG_VALUE( "debug", "release" )
            , MENGINE_MASTER_RELEASE_VALUE( "master", "RC" )
            , MENGINE_BUILD_PUBLISH_VALUE( "publish", "manual" )
            , buildVersion
            , buildNumber
            , engineGitSHA1
        );

        LoggerMessage msg;
        msg.category = MENGINE_CODE_LIBRARY;
        msg.timestamp = Helper::getLocalTimestamp();
        msg.thread = Helper::getCurrentThreadName();
        msg.level = LM_MESSAGE;
        msg.flag = LFLAG_SHORT;
        msg.filter = LFILTER_NONE;
        msg.color = LCOLOR_GREEN;
        msg.file = "";
        msg.function = "";
        msg.line = 0;
        msg.data = loggerLevelMessage;
        msg.size = loggerLevelMessageLen;

        LoggerRecordInterfacePtr record = this->makeLoggerRecord( msg, MENGINE_DOCUMENT_FACTORABLE );

        this->logHistory_( record );

        ThreadSharedMutexInterfacePtr mutexLogger = Helper::createThreadSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexLogger, "invalid create shared mutex" );

        m_mutexLogger = mutexLogger;

        ThreadSharedMutexInterfacePtr mutexHistory = Helper::createThreadSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexHistory, "invalid create shared mutex" );

        m_mutexHistory = mutexHistory;

        ThreadMutexInterfacePtr mutexMessageBlock = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutexMessageBlock, "invalid create mutex" );

        m_mutexMessageBlock = mutexMessageBlock;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CONFIGS_LOAD, &LoggerService::notifyConfigsLoad_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE, &LoggerService::notifyBootstrapperRunCompete_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CONFIGS_LOAD );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );

        m_memoryOldLog = nullptr;
        m_currentContentLog = nullptr;

        m_mutexHistory = nullptr;
        m_mutexMessageBlock = nullptr;
        m_mutexLogger = nullptr;

        m_loggers.clear();
        m_verboses.clear();
        m_history.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryLoggerRecord );
        m_factoryLoggerRecord = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::_stopService()
    {
        m_historically = false;

        this->flushMessages();
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
        m_history.shrink_to_fit();
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
    LoggerRecordInterfacePtr LoggerService::makeLoggerRecord( const LoggerMessage & _message, const DocumentInterfacePtr & _doc )
    {
        LoggerRecordPtr record = m_factoryLoggerRecord->createObject( _doc );

        record->initialize( _message );

        return record;
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

        MENGINE_ASSERTION_VALIDATE_UTF8( _message.data, _message.size );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_BEGIN, _message );
#endif

        LoggerRecordInterfacePtr record = this->makeLoggerRecord( _message, MENGINE_DOCUMENT_FACTORABLE );

        this->logMessage_( record );
        this->logHistory_( record );

        if( (_message.filter & LFILTER_EXCEPTION) != 0 )
        {
            this->flushMessages();
        }

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        NOTIFICATION_NOTIFY( NOTIFICATOR_LOGGER_END, _message );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logMessage_( const LoggerRecordInterfacePtr & _record )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexMessageBlock );

        LoggerMessage message;
        _record->getMessage( &message );

        ELoggerLevel level = message.level;

        uint32_t statisticId = m_staticsticLevel[level];

        MENGINE_UNUSED( statisticId );

        STATISTIC_INC_INTEGER( statisticId );

        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutexLogger );

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            if( logger->validMessage( _record ) == false )
            {
                continue;
            }

            logger->log( _record );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::logHistory_( const LoggerRecordInterfacePtr & _record )
    {
        if( m_historically == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexHistory );

        m_history.emplace_back( _record );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::notifyConfigsLoad_()
    {
        VectorConstString verboses;
        CONFIG_VALUES( "Engine", "Verboses", &verboses );

        m_verboses.insert( m_verboses.begin(), verboses.begin(), verboses.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::notifyBootstrapperRunCompete_()
    {
        this->setHistorically( false );

        this->clearHistory();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::lockMessages()
    {
        if( m_mutexMessageBlock == nullptr )
        {
            return;
        }

        m_mutexMessageBlock->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unlockMessages()
    {
        if( m_mutexMessageBlock == nullptr )
        {
            return;
        }

        m_mutexMessageBlock->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::flushMessages()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        for( const LoggerInterfacePtr & logger : m_loggers )
        {
            logger->flush();
        }
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

        for( const LoggerRecordInterfacePtr & record : m_history )
        {
            if( _logger->validMessage( record ) == false )
            {
                continue;
            }

            _logger->log( record );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::loadOldLogMemory( const ContentInterfacePtr & _content )
    {
        if( _content->exist( false ) == false )
        {
            return false;
        }

        MemoryInterfacePtr memory = _content->createMemoryFileString( false, false, MENGINE_DOCUMENT_FACTORABLE );

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
    void LoggerService::setCurrentContentLog( const ContentInterfacePtr & _content )
    {
        m_currentContentLog = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & LoggerService::getCurrentContentLog() const
    {
        return m_currentContentLog;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoggerService::registerLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_ASSERTION_FATAL( StdAlgorithm::find( m_loggers.begin(), m_loggers.end(), _logger ) == m_loggers.end(), "already register logger" );

        if( _logger->initializeLogger() == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        m_loggers.emplace_back( _logger );

        StdAlgorithm::sort( m_loggers.begin(), m_loggers.end(), []( const LoggerInterfacePtr & _l, const LoggerInterfacePtr & _r )
        {
            return _l->getVerboseLevel() > _r->getVerboseLevel();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerService::unregisterLogger( const LoggerInterfacePtr & _logger )
    {
        MENGINE_ASSERTION_FATAL( StdAlgorithm::find( m_loggers.begin(), m_loggers.end(), _logger ) != m_loggers.end(), "not found logger" );

        _logger->flush();

        _logger->finalizeLogger();

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexLogger );

        VectorLoggers::iterator it_find =
            StdAlgorithm::find( m_loggers.begin(), m_loggers.end(), _logger );

        m_loggers.erase( it_find );
    }
    //////////////////////////////////////////////////////////////////////////
}
