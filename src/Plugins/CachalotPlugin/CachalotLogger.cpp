#include "CachalotLogger.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/HttpServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VectorHelper.h"
#include "Kernel/JSON2Helper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::CachalotLogger()
        : m_status( ECS_NONE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::~CachalotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::setDSN( const String & _dsn )
    {
        m_dsn = _dsn;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & CachalotLogger::getDSN() const
    {
        return m_dsn;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotLogger::_initializeLogger()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        m_mutex = mutex;

        m_status = ECS_CONNECT;

        HTTP_SERVICE()
            ->ping( m_dsn, 1000, [this]( bool _successful )
        {
            if( _successful == false )
            {
                this->stop();

                return;
            }

            this->start();
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_finalizeLogger()
    {
        this->stop();
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        if( m_status == ECS_DISCONNECT )
        {
            return;
        }

        m_mutex->lock();
        m_messages.emplace_back( _record );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::start()
    {
        m_status = ECS_READY;

        uint32_t CachalotPlugin_LoggerTime = CONFIG_VALUE_INTEGER( "CachalotPlugin", "Time", 2000 );

        ThreadIdentityInterfacePtr thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGCachalot" ), ETP_BELOW_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            this->process( _runner );

            return true;
        }, CachalotPlugin_LoggerTime, MENGINE_DOCUMENT_FACTORABLE );

        m_thread = thread;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::process( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        MENGINE_UNUSED( _runner );

        if( m_status != ECS_READY )
        {
            return;
        }

        uint32_t CachalotPlugin_BatchCount = CONFIG_VALUE_INTEGER( "CachalotPlugin", "BatchCount", 50 );

        m_mutex->lock();
        VectorMessages messages = Helper::exciseVector( m_messages, CachalotPlugin_BatchCount );
        m_mutex->unlock();

        if( messages.empty() == true )
        {
            return;
        }

        //ToDo: setup user id
        //Char user_id[256] = {'\0'};

        Char install_id[MENGINE_ENVIRONMENT_INSTALLID_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallId( install_id );

        int64_t install_timestamp = ENVIRONMENT_SERVICE()
            ->getInstallTimestamp();

        Char install_version[MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getInstallVersion( install_version );

        int64_t install_rnd = ENVIRONMENT_SERVICE()
            ->getInstallRND();

        int64_t session_index = ENVIRONMENT_SERVICE()
            ->getSessionIndex();

        Timestamp live = PLATFORM_SERVICE()
            ->getPlatfomTime();

        Char device_model[MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getDeviceModel( device_model );

        Char os_family[MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getOSFamily( os_family );

        Char os_version[MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getOSVersion( os_version );

        Char build_bundle[MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME + 1] = {'\0'};
        ENVIRONMENT_SERVICE()
            ->getBundleId( build_bundle );

        const Char * build_version = Helper::getBuildVersion();
        uint64_t build_number = Helper::getBuildNumber();

        js_element_t * j = Helper::createJSON2();

        MENGINE_ASSERTION_MEMORY_PANIC( j, "invalid create json" );

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

            //ToDo: setup user id
            //js_object_add_field_string( j, jrecord, JS_CONST_STRING( "user.id" ), user_id );

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

            js_object_add_field_string( j, jattributes, JS_CONST_STRING( "install.key" ), install_id );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "install.timestamp" ), install_timestamp );
            js_object_add_field_string( j, jattributes, JS_CONST_STRING( "install.version" ), install_version );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "install.rnd" ), install_rnd );
            js_object_add_field_integer( j, jattributes, JS_CONST_STRING( "session.index" ), session_index );
        }

        m_dataAux.clear();

        if( Helper::writeJSON2DataCompact( j, &m_dataAux ) == false )
        {
            return;
        }

        Helper::destroyJSON2( j );

        const HttpHeaders & headers = HTTP_SERVICE()
            ->getApplicationJSONHeaders();

        HttpRequestId id = HTTP_SERVICE()
            ->headerData( m_dsn, headers, m_dataAux, 2000, EHRF_LOW_PRIORITY, HttpReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::stop()
    {
        if( m_status == ECS_DISCONNECT )
        {
            return;
        }

        m_status = ECS_DISCONNECT;

        m_mutex->lock();
        m_messages.clear();
        m_mutex->unlock();

        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        bool successful = _response->isSuccessful();

        if( successful == false )
        {
            this->stop();

            return;
        }

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
