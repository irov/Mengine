#include "DevToDebugLogger.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/HttpServiceInterface.h"

#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/VectorHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadHelper.h"

#define DEVTODEBUGLOGGER_THREAD_NAME "DevToDebugLogger"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugLogger::DevToDebugLogger()
        : m_status( EDTDS_NONE )
        , m_timerId( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugLogger::~DevToDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::setWorkerURL( const String & _workerURL )
    {
        m_workerURL = _workerURL;

        m_status = EDTDS_CONNECT;

        HTTP_SERVICE()
            ->ping( m_workerURL, 1000, [this]( bool _successful )
        {
            if( _successful == false )
            {
                this->stop();

                return;
            }

            this->start();
        }, MENGINE_DOCUMENT_FACTORABLE );
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

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        m_mutex = mutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_finalizeLogger()
    {   
        this->stop();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        if( m_status == EDTDS_DISCONNECT )
        {
            return;
        }

        m_mutex->lock();
        m_messages.emplace_back( _record );
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
    void DevToDebugLogger::start()
    {
        m_status = EDTDS_READY;

        uint32_t DevToDebug_LoggerTime = CONFIG_VALUE_INTEGER( "DevToDebugPlugin", "LoggerTime", 2000 );

        ThreadIdentityInterfacePtr thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGD2DLogger" ), ETP_BELOW_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            this->process( _runner );

            return true;
        }, DevToDebug_LoggerTime, MENGINE_DOCUMENT_FACTORABLE );

        m_thread = thread;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::stop()
    {
        if( m_status == EDTDS_DISCONNECT )
        {
            return;
        }

        m_status = EDTDS_DISCONNECT;

        m_mutex->lock();
        m_messages.clear();
        m_mutex->unlock();

        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::process( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        MENGINE_UNUSED( _runner );

        if( m_status != EDTDS_READY )
        {
            return;
        }

        uint32_t DevToDebug_LoggerBatchCount = CONFIG_VALUE_INTEGER( "DevToDebugPlugin", "LoggerBatchCount", 1024 );

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
            ->headerData( m_workerURL, headers, data, 2000, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
