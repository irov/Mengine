#include "CachalotLogger.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadWorkerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VectorHelper.h"
#include "Kernel/JSON2Helper.h"
#include "Kernel/BuildMode.h"

#include "Config/StdString.h"

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

        LOGGER_MESSAGE( "Cachalot DSN: %s"
            , CachalotPlugin_DSN.c_str()
        );

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
    void CachalotLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        m_mutex->lock();
        m_messages.emplace_back( _record );
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

        Char session_id[64] = {'\0'};
        size_t session_id_length = PLATFORM_SERVICE()
            ->getSessionId( session_id );

        if( session_id_length == 0 )
        {
            return;
        }

        Char install_key[64] = {'\0'};
        size_t install_key_length = PLATFORM_SERVICE()
            ->getInstallKey( install_key );

        int64_t install_timestamp = PLATFORM_SERVICE()
            ->getInstallTimestamp();

        Char install_version[64] = {'\0'};
        size_t install_version_length = PLATFORM_SERVICE()
            ->getInstallVersion( install_version );

        int64_t install_rnd = PLATFORM_SERVICE()
            ->getInstallRND();

        int64_t session_index = PLATFORM_SERVICE()
            ->getSessionIndex();

        Timestamp live = PLATFORM_SERVICE()
            ->getPlatfomTime();

        Char device_model[MENGINE_PLATFORM_DEVICE_MODEL_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getDeviceModel( device_model );

        Char os_family[MENGINE_PLATFORM_OS_FAMILY_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getOsFamily( os_family );

        Char os_version[MENGINE_PLATFORM_OS_VERSION_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getOsVersion( os_version );
        
        Char build_bundle[MENGINE_PLATFORM_BUNDLEID_MAXNAME] = {'\0'};
        PLATFORM_SERVICE()
            ->getBundleId( build_bundle );

        const Char * build_version = Helper::getBuildVersion();
        uint64_t build_number = Helper::getBuildNumber();

        js_element_t * j = Helper::createJSON2();

        MENGINE_ASSERTION_MEMORY_PANIC( j );

        js_element_t * jrecords;
        if( js_object_add_field_array( j, j, JS_CONST_STRING( "records" ), &jrecords ) == JS_FAILURE )
        {
            return;
        }

        for( const LoggerRecordInterfacePtr & record : messages )
        {
            LoggerMessage message;
            record->getMessage( &message );

            js_element_t * jrecord;
            if( js_array_push_object( j, jrecords, &jrecord ) == JS_FAILURE )
            {
                return;
            }

            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "user.id" ), JS_MAKE_STRING( session_id, session_id_length ) );

            switch( message.level )
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

            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "service" ), message.category );

            if( message.size < 4096 )
            {
                js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "message" ), JS_MAKE_STRING( message.data, message.size ) );
            }
            else
            {
                js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "message" ), JS_MAKE_STRING( message.data, 4096 ) );
            }

            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "build.version" ), build_version );
            
            
#if defined(MENGINE_MASTER_RELEASE_ENABLE)
            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "build.environment" ), JS_CONST_STRING( "master" ) );
#else
            js_object_add_field_stringn( j, jrecord, JS_CONST_STRING( "build.environment" ), JS_CONST_STRING( "dev" ) );
#endif

            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "build.bundle" ), build_bundle );
            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "build.version" ), build_version );
            js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "build.number" ), build_number );
            js_object_add_field_boolean( j, jrecord, JS_CONST_STRING( "build.release" ), MENGINE_RELEASE_VALUE( JS_TRUE, JS_FALSE ) );

            js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "timestamp" ), message.timestamp );
            js_object_add_field_integer( j, jrecord, JS_CONST_STRING( "live" ), live );

            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "device.model" ), device_model );
            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "os.family" ), os_family );
            js_object_add_field_string( j, jrecord, JS_CONST_STRING( "os.version" ), os_version );

            js_element_t * jattributes;
            if( js_object_add_field_object( j, jrecord, JS_CONST_STRING( "attributes" ), &jattributes ) == JS_FAILURE )
            {
                return;
            }

            js_object_add_field_stringn( j, jattributes, JS_CONST_STRING( "install.key" ), JS_MAKE_STRING( install_key, install_key_length ) );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "install.timestamp" ), install_timestamp );
            js_object_add_field_stringn( j, jattributes, JS_CONST_STRING( "install.version" ), JS_MAKE_STRING( install_version, install_version_length ) );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "install.rnd" ), install_rnd );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "session.index" ), session_index );
        }

        m_dataAux.clear();

        if( Helper::writeJSON2DataCompact( j, &m_dataAux ) == false )
        {
            return;
        }

        const HttpRequestHeaders & headers = HTTP_SYSTEM()
            ->getApplicationJSONHeaders();

        HttpRequestId id = HTTP_SYSTEM()
            ->headerData( m_dsn, headers, MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY, false, m_dataAux, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

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
