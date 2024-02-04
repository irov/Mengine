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

#include "Kernel/JSON2Helper.h"

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

        m_dsn.clear();
        m_messages.clear();
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

        js_element_t * j = Helper::createJSON2();

        MENGINE_ASSERTION_MEMORY_PANIC( j );

        js_element_t * jrecords;
        if( js_object_add_field_array( j, j, JS_CONST_STRING( "records" ), &jrecords ) == JS_FAILURE )
        {
            return;
        }

        for( const LoggerRecord & record : messages )
        {
            js_element_t * jrecord;
            if( js_array_push_object( j, jrecords, &jrecord ) == JS_FAILURE )
            {
                return;
            }

            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "user.id" ), JS_MAKE_STRING( sessionId.c_str(), sessionId.size() ) );

            switch( record.level )
            {
            case LM_FATAL:
            case LM_ERROR:
                {
                    js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "level" ), 0 );
                }break;
            case LM_WARNING:
                {
                    js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "level" ), 1 );
                }break;
            case LM_MESSAGE_RELEASE:
            case LM_MESSAGE:
                {
                    js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "level" ), 2 );
                }break;
            case LM_INFO:
            case LM_DEBUG:
            case LM_VERBOSE:
                {
                    js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "level" ), 3 );
                }break;
            default:
                break;
            }

            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "service" ), JS_MAKE_STRING( record.category, MENGINE_STRLEN( record.category ) ) );
            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "message" ), JS_MAKE_STRING( record.data.data(), record.data.size() ) );

            if( record.file != nullptr )
            {
                js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "file" ), JS_MAKE_STRING( record.file, MENGINE_STRLEN( record.file ) ) );
            }

            js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "line" ), record.line );
            js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "timestamp" ), record.timestamp );
        }

        Data data;
        if( Helper::writeJSON2DataCompact( j, &data ) == false )
        {
            return;
        }

        const HttpRequestHeaders & headers = HTTP_SYSTEM()
            ->getApplicationJSONHeaders();

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
