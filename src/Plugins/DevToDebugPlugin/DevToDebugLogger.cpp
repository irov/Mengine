#include "DevToDebugLogger.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/VectorHelper.h"

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
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        uint32_t DevToDebug_LoggerTime = CONFIG_VALUE( "DevToDebugPlugin", "LoggerTime", 2000 );

        if( Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "DevToDebugLogger" ), ETP_BELOW_NORMAL, DevToDebug_LoggerTime, nullptr, [this]()
        {
            this->process();
        }, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::_finalizeLogger()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "DevToDebugLogger" ) );

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::log( const LoggerMessage & _message )
    {
        const Char * data = _message.data;
        size_t size = _message.size;

        MessageDesc desc;

        Char loggerTimestamp[1024] = {'\0'};
        Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", loggerTimestamp, 0, 1024 );

        desc.timestamp = loggerTimestamp;
        desc.tag = _message.category;
        desc.file = _message.file;
        desc.line = _message.line;

        if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == true )
        {
            const ConstString & threadName = THREAD_SERVICE()
                ->getCurrentThreadName();

            desc.threadName = threadName;
        }
        
        desc.data.assign( data, data + size );

        switch( _message.level )
        {
        case LM_SILENT:
            return;
        case LM_FATAL:
            desc.level = 'F';
            break;
        case LM_MESSAGE_RELEASE:
            desc.level = 'R';
            break;
        case LM_ERROR:
            desc.level = 'E';
            break;
        case LM_WARNING:
            desc.level = 'W';
            break;
        case LM_MESSAGE:
            desc.level = 'M';
            break;
        case LM_INFO:
            desc.level = 'I';
            break;
        case LM_DEBUG:
            desc.level = 'D';
            break;
        case LM_VERBOSE:
            desc.level = 'V';
            break;
        }

        m_mutex->lock();
        m_messages.emplace_back( desc );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugLogger::validMessage( const LoggerMessage & _message ) const
    {
        MENGINE_UNUSED( _message );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::flush()
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

        for( const MessageDesc & desc : messages )
        {
            jpp::object j_desc = jpp::make_object();

            j_desc.set( "ts", desc.timestamp );
            j_desc.set( "tag", desc.tag );
            j_desc.set( "file", desc.file );
            j_desc.set( "line", desc.line );
            j_desc.set( "thread", desc.threadName );
            j_desc.set( "data", desc.data );
            j_desc.set( "level", desc.level );

            jlog.push_back( j_desc );
        }

        jlogger.set( "log", jlog );

        j.set( "logger", jlogger );

        HttpRequestHeaders headers;
        headers.push_back( "Content-Type:application/json" );

        String data;
        Helper::writeJSONStringCompact( j, &data );

        HttpRequestId id = HTTP_SYSTEM()
            ->headerData( m_workerURL, headers, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, false, data, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
