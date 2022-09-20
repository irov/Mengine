#include "DevToDebugLogger.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"

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

        uint32_t DevToDebug_LoggerTime = CONFIG_VALUE( "DevToDebugPlugin", "LoggerTime", 500 );

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
    void DevToDebugLogger::log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );

        MessageDesc desc;
        desc.timestamp = "";
        desc.tag = ConstString::none();
        desc.data.assign( _data, _data + _size );

        m_mutex->lock();
        m_messages.emplace_back( desc );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::flush()
    {

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

        m_mutex->lock();
        VectorMessages messages = std::move( m_messages );
        m_mutex->unlock();

        jpp::array j_log = jpp::make_array();

        for( const MessageDesc & desc : messages )
        {
            jpp::object j_desc = jpp::make_object();

            j_desc.set( "ts", desc.timestamp );
            j_desc.set( "tag", desc.tag );
            j_desc.set( "data", desc.data );

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

        LOGGER_INFO( "devtodebug", "Process: data %s [id %u]"
            , data.c_str()
            , id
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugLogger::onHttpRequestComplete( const cURLResponseData & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
