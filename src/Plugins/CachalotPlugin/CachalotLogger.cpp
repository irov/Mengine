#include "CachalotLogger.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PreferencesSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadWorkerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VectorHelper.h"

#include "Kernel/JSONHelper.h"

#include "Config/StdString.h"

#include <clocale>
#include <iostream>

#include <io.h>
#include <fcntl.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::CachalotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::~CachalotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotLogger::_initializeLogger()
    {
        String CachalotPlugin_DSN = CONFIG_VALUE( "CachalotPlugin", "DSN", "" );

        if( CachalotPlugin_DSN.empty() == true )
        {
            LOGGER_ERROR( "invalid DSN" );

            return false;
        }

        m_dsn = CachalotPlugin_DSN;

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        uint32_t CachalotPlugin_LoggerTime = CONFIG_VALUE( "CachalotPlugin", "Time", 2000 );

        if( Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "CachalotLogger" ), ETP_BELOW_NORMAL, CachalotPlugin_LoggerTime, nullptr, [this]()
        {
            this->process();
        }, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_finalizeLogger()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "CachalotLogger" ) );

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_log( const LoggerMessage & _message )
    {
        LoggerRecord record;
        Helper::recordLoggerMessage( _message, &record );

        m_mutex->lock();
        m_messages.emplace_back( record );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::process()
    {
        uint32_t CachalotPlugin_BatchCount = CONFIG_VALUE( "CachalotPlugin", "BatchCount", 50 );

        m_mutex->lock();
        VectorMessages messages = Helper::exciseVector( m_messages, CachalotPlugin_BatchCount );
        m_mutex->unlock();

        if( messages.empty() == true )
        {
            return;
        }

        ConstString sessionId = PREFERENCES_SYSTEM()
            ->getPreferenceConstString( "session_id", ConstString::none() );

        jpp::object j = jpp::make_object();

        jpp::array jlog = jpp::make_array();

        for( const LoggerRecord & record : messages )
        {
            jpp::object j_desc = jpp::make_object();

            j_desc.set( "user.id", sessionId );
            switch( record.level )
            {
            case LM_FATAL:
            case LM_ERROR:
                {
                    j_desc.set( "level", 0 );
                }break;
            case LM_WARNING:
                {
                    j_desc.set( "level", 1 );
                }break;
            case LM_MESSAGE_RELEASE:
            case LM_MESSAGE:
                {
                    j_desc.set( "level", 2 );
                }break;
            case LM_INFO:
            case LM_DEBUG:
            case LM_VERBOSE:
                {
                    j_desc.set( "level", 3 );
                }break;
            default:
                break;
            }

            j_desc.set( "service", record.category );
            j_desc.set( "message", record.data );
            j_desc.set( "file", record.file );
            j_desc.set( "line", record.line );
            j_desc.set( "timestamp", record.timestamp );

            jlog.push_back( j_desc );
        }

        j.set( "records", jlog );

        const HttpRequestHeaders & headers = HTTP_SYSTEM()
            ->getApplicationJSONHeaders();

        String data;
        Helper::writeJSONStringCompact( j, &data );

        HttpRequestId id = HTTP_SYSTEM()
            ->headerData( m_dsn, headers, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, false, data, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
