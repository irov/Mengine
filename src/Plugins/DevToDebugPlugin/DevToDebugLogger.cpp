#include "DevToDebugLogger.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/HttpServiceInterface.h"

#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ThreadWorkerHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/VectorHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#define DEVTODEBUGLOGGER_THREAD_NAME "DevToDebugLogger"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugLogger::DevToDebugLogger()
        : m_timerId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugLogger::~DevToDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::setWorkerURL( const String & _workerURL )
    {
        m_mutex->lock();
        m_workerURL = _workerURL;
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    const String & DevToDebugLogger::getWorkerURL() const
    {
        return m_workerURL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugLogger::_initializeLogger()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        uint32_t DevToDebug_LoggerTime = CONFIG_VALUE( "DevToDebugPlugin", "LoggerTime", 2000 );

        if( Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL_I( DEVTODEBUGLOGGER_THREAD_NAME ), ETP_BELOW_NORMAL, DevToDebug_LoggerTime, ThreadWorkerInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_finalizeLogger()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL_I( DEVTODEBUGLOGGER_THREAD_NAME ) );
        
        m_mutex = nullptr;

        m_messages.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        m_mutex->lock();

        if( m_workerURL.empty() == false )
        {
            m_messages.emplace_back( _record );
        }

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugLogger::validMessage( const LoggerRecordInterfacePtr & _record ) const
    {
        MENGINE_UNUSED( _record );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_flush()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::process()
    {
        m_mutex->lock();
        bool wait = m_workerURL.empty();
        m_mutex->unlock();

        if( wait == true )
        {
            return;
        }

        uint32_t DevToDebug_LoggerBatchCount = CONFIG_VALUE( "DevToDebugPlugin", "LoggerBatchCount", 1024 );

        m_mutex->lock();
        VectorMessages messages = Helper::exciseVector( m_messages, DevToDebug_LoggerBatchCount );
        m_mutex->unlock();

        jpp::object j = jpp::make_object();

        jpp::object jlogger = jpp::make_object();

        jpp::array jlog = jpp::make_array();

        for( const LoggerRecordInterfacePtr & record : messages )
        {
            LoggerMessage message;
            record->getMessage( &message );

            jpp::object j_desc = jpp::make_object();

            Char loggerTimestamp[1024 + 1] = {'\0'};
            Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", loggerTimestamp, 0, 1024 );

            j_desc.set( "ts", loggerTimestamp );
            j_desc.set( "tag", message.category );
            j_desc.set( "file", message.function );
            j_desc.set( "line", message.line );
            j_desc.set( "thread", message.threadName );
            j_desc.set( "data", message.data );

            Char level = '-';

            switch( message.level )
            {
            case LM_SILENT:
                return;
            case LM_FATAL:
                level = 'F';
                break;
            case LM_MESSAGE_RELEASE:
                level = 'R';
                break;
            case LM_ERROR:
                level = 'E';
                break;
            case LM_WARNING:
                level = 'W';
                break;
            case LM_MESSAGE:
                level = 'M';
                break;
            case LM_INFO:
                level = 'I';
                break;
            case LM_DEBUG:
                level = 'D';
                break;
            case LM_VERBOSE:
                level = 'V';
                break;
            }           

            j_desc.set( "level", level );

            jlog.push_back( j_desc );
        }

        jlogger.set( "log", jlog );

        j.set( "logger", jlogger );

        const HttpHeaders & headers = HTTP_SERVICE()
            ->getApplicationJSONHeaders();

        Data data;
        Helper::writeJSONDataCompact( j, &data );

        HttpRequestId id = HTTP_SERVICE()
            ->headerData( m_workerURL, headers, data, 2000, EHRE_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onThreadWorkerUpdate( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugLogger::onThreadWorkerWork( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        this->process();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onThreadWorkerDone( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
