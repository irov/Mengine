#include "DevToDebugLogger.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformInterface.h"

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
    void DevToDebugLogger::setLoggerURL( const String & _workerURL )
    {
        m_mutex->lock();
        m_loggerURL = _workerURL;
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    const String & DevToDebugLogger::getLoggerURL() const
    {
        return m_loggerURL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugLogger::_initializeLogger()
    {
        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        uint32_t DevToDebug_LoggerTime = CONFIG_VALUE( "DevToDebugPlugin", "LoggerTime", 1000 );

        Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "DevToDebugLogger" ), ETP_BELOW_NORMAL, DevToDebug_LoggerTime, nullptr, [this]()
        {
            this->process();
        }, MENGINE_DOCUMENT_FACTORABLE );

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
        Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", loggerTimestamp, 1024 );

        desc.timestamp = loggerTimestamp;
        desc.tag = _message.category;
        desc.data.assign( data, data + size );

        switch( _message.level )
        {
        case LM_SILENT:
            return;
        case LM_FATAL:
        case LM_CRITICAL:
            desc.level = 'F';
            break;
        case LM_MESSAGE_RELEASE:
            desc.level = 'R';
            break;
        case LM_ERROR:
            desc.level = 'E';
            break;
        case LM_PERFOMANCE:
        case LM_STATISTIC:
        case LM_WARNING:
            desc.level = 'W';
            break;
        case LM_MESSAGE:
            desc.level = 'M';
            break;
        case LM_INFO:
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
        bool wait = m_loggerURL.empty();
        m_mutex->unlock();

        if( wait == true )
        {
            return;
        }

        uint32_t DevToDebug_LoggerBatchCount = CONFIG_VALUE( "DevToDebugPlugin", "LoggerBatchCount", 1024 );

        m_mutex->lock();
        VectorMessages messages = Helper::exciseVector( m_messages, DevToDebug_LoggerBatchCount );
        m_mutex->unlock();

        jpp::array j_log = jpp::make_array();

        for( const MessageDesc & desc : messages )
        {
            jpp::object j_desc = jpp::make_object();

            j_desc.set( "ts", desc.timestamp );
            j_desc.set( "tag", desc.tag );
            j_desc.set( "data", desc.data );
            j_desc.set( "level", desc.level );

            j_log.push_back( j_desc );
        }

        jpp::object j = jpp::make_object();

        j.set( "log", j_log );

        cURLHeaders headers;
        headers.push_back( "Content-Type:application/json" );

        String data;
        Helper::writeJSONStringCompact( j, &data );

        HttpRequestID id = CURL_SERVICE()
            ->headerData( m_loggerURL, headers, MENGINE_CURL_TIMEOUT_INFINITY, false, data, cURLReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onHttpRequestComplete( const cURLResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
