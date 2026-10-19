#include "AndroidWebViewApplication.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/WebViewBootstrapperInterface.h"
#include "Interface/WebViewBridgeServiceInterface.h"
#include "Interface/WebViewPlatformExtensionInterface.h"
#include "Interface/WebViewPlatformServiceInterface.h"

#include "Environment/Android/AndroidPluginHelper.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidJSONHelper.h"
#include "Environment/Android/AndroidLogger.h"
#include "Environment/Android/AndroidWebViewPlatformServiceExtensionInterface.h"

#include "Runtime/MengineRuntime.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Base64.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/StringHelper.h"

#if defined(MENGINE_WEBVIEW_MCP)
#   include "Kernel/ProxyLogger.h"
#   include "Kernel/ThreadMutexHelper.h"
#   include "Kernel/ThreadMutexScope.h"
#endif

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AndroidKernelService );
SERVICE_EXTERN( AndroidAssetService );
SERVICE_EXTERN( EnvironmentService );
SERVICE_EXTERN( WebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const uint32_t ANDROID_WEBVIEW_MAX_EVENTS_PER_TICK = 256;
    //////////////////////////////////////////////////////////////////////////
    AndroidWebViewApplication::AndroidWebViewApplication()
#if defined(MENGINE_WEBVIEW_MCP)
        : m_logSequence( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidWebViewApplication::~AndroidWebViewApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewApplication::bootstrap( JNIEnv * _env )
    {
        if( Mengine_JNI_Initialize( _env ) == JNI_FALSE )
        {
            return false;
        }

        Configuration configuration;
        configuration.silentDialog = true;
        StdString::strcpy_safe( configuration.dataDirectory, "WebView/", MENGINE_MAX_PATH );

        if( API_MengineCreate( &configuration ) == nullptr )
        {
            return false;
        }

        if( API_MengineBootstrap() == false )
        {
            return false;
        }

        LoggerInterfacePtr logger = Helper::makeFactorableUnique<AndroidLogger>( nullptr );
        logger->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( logger ) == false )
        {
            return false;
        }

        m_logger = logger;

#if defined(MENGINE_WEBVIEW_MCP)
        m_logMutex = Helper::createThreadMutex( nullptr );

        if( m_logMutex == nullptr )
        {
            return false;
        }

        m_logs = jpp::make_array();
        ProxyLoggerPtr collector = Helper::makeFactorableUnique<ProxyLogger>( nullptr );
        collector->setLambda( [this]( const LoggerRecordInterfacePtr & _record )
        {
            LoggerMessage message;
            _record->getMessage( &message );

            const Char * category = message.category != nullptr ? message.category : "";
            String text( message.data, message.size );
            uint32_t level = static_cast<uint32_t>(message.level);
            jpp::object record = jpp::make_object();
            record.set( "timestamp", message.timestamp );
            record.set( "level", level );
            record.set( "category", category );
            record.set( "message", text );
            MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );

            if( m_logs.size() >= 5000 )
            {
                m_logs.remove_front();
            }

            uint64_t sequence = ++m_logSequence;
            record.set( "sequence", sequence );
            m_logs.push_back( record );
        } );
        collector->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( collector ) == false )
        {
            return false;
        }

        m_logCollector = collector;
#endif

        if( SERVICE_CREATE_SAFE( AndroidKernelService, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewApplication::run()
    {
        if( Mengine_JNI_SetupThread() == JNI_FALSE )
        {
            return false;
        }

        if( SERVICE_CREATE_SAFE( AndroidAssetService, nullptr ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE_SAFE( EnvironmentService, nullptr ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE_SAFE( WebViewPlatformService, nullptr ) == false )
        {
            return false;
        }

        JNIEnv * env = Mengine_JNI_GetEnv();
        jstring directory = (jstring)Helper::AndroidCallObjectPluginMethod( env, "AndroidWebViewPlugin", "getWebViewTemporaryDirectory", "()Ljava/lang/String;" );
        const Char * path = env->GetStringUTFChars( directory, nullptr );
        FilePath temporary = Helper::stringizeFilePath( path );
        env->ReleaseStringUTFChars( directory, path );
        env->DeleteLocalRef( directory );

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr, nullptr, temporary, STRINGIZE_STRING_LOCAL( "global" ), nullptr, true, nullptr ) == false )
        {
            return false;
        }

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "webview" ), nullptr, nullptr, STRINGIZE_FILEPATH_LOCAL( "WebView/" ), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, nullptr ) == false )
        {
            return false;
        }

        if( API_MengineRun() == false )
        {
            return false;
        }

        if( this->registerApplicationHandlers_() == false )
        {
            return false;
        }

        jclass eventClass = Helper::AndroidEnvFindClass( env, "org/Mengine/Base/MengineWebViewEvent" );
        jfieldID eventIdField = Mengine_JNI_GetFieldID( env, eventClass, "id", "I" );
        jfieldID eventDataField = Mengine_JNI_GetFieldID( env, eventClass, "data", "Lorg/json/JSONObject;" );
        Mengine_JNI_DeleteLocalRef( env, eventClass );

        Timestamp previous = Helper::getPlatformTimestamp();
        bool quit = false;

        while( quit == false )
        {
            bool changed = false;

            for( uint32_t i = 0; i != ANDROID_WEBVIEW_MAX_EVENTS_PER_TICK; ++i )
            {
                jint localReferenceCapacity = 16;
                if( Mengine_JNI_PushLocalFrame( env, localReferenceCapacity ) != JNI_OK )
                {
                    Helper::AndroidEnvExceptionCheck( env );

                    return false;
                }

                jobject javaEvent = Helper::AndroidCallObjectPluginMethod( env, "AndroidWebViewPlugin", "pollWebViewEvent", "()Lorg/Mengine/Base/MengineWebViewEvent;" );

                if( javaEvent == nullptr )
                {
                    Mengine_JNI_PopLocalFrame( env, nullptr );

                    break;
                }

                EAndroidWebViewEvent eventId = Mengine_JNI_GetEnumField<EAndroidWebViewEvent>( env, javaEvent, eventIdField );

                if( eventId == EAndroidWebViewEvent::QUIT )
                {
                    Mengine_JNI_PopLocalFrame( env, nullptr );
                    quit = true;

                    break;
                }

                jobject data = Mengine_JNI_GetObjectField( env, javaEvent, eventDataField );
                bool successful = this->processEvent_( env, eventId, data );
                Mengine_JNI_PopLocalFrame( env, nullptr );

                if( successful == false )
                {
                    return false;
                }

                changed = true;
            }

            if( quit == true )
            {
                break;
            }

            Timestamp now = Helper::getPlatformTimestamp();
            float milliseconds = static_cast<float>(now - previous);
            previous = now;
            WEBVIEW_BOOTSTRAPPER_SERVICE()
                ->tick( milliseconds );

            if( changed == true )
            {
                this->publishStatus_();
            }

            THREAD_SYSTEM()
                ->sleep( 10 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewApplication::processEvent_( JNIEnv * _env, EAndroidWebViewEvent _id, jobject _event )
    {
        AndroidWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
            ->getUnknown();
        jclass jsonClass = Mengine_JNI_GetClassJSONObject( _env );
        platform->processEvent( _env, jsonClass, _id, _event );

        switch( _id )
        {
        case EAndroidWebViewEvent::ATTACH:
            {
                const FileGroupInterfacePtr & content = FILE_SERVICE()
                    ->getFileGroup( STRINGIZE_STRING_LOCAL( "webview" ) );

                if( platform->openContent( content, STRINGIZE_FILEPATH_LOCAL( "index.html" ) ) == false )
                {
                    return false;
                }
            }break;
        case EAndroidWebViewEvent::RELOAD:
            {
                platform->reload();
            }break;
#if defined(MENGINE_WEBVIEW_MCP)
        case EAndroidWebViewEvent::LOGS_READ:
            {
                String id = Helper::AndroidGetJavaJSONObjectString( _env, jsonClass, _event, "id", "" );
                jlong afterValue = Helper::AndroidGetJavaJSONObjectLong( _env, jsonClass, _event, "after", 0 );
                jint requestedLimit = Helper::AndroidGetJavaJSONObjectInt( _env, jsonClass, _event, "limit", 500 );
                uint64_t after = afterValue > 0 ? static_cast<uint64_t>(afterValue) : 0;
                size_t limit = static_cast<size_t>(requestedLimit);
                uint64_t next = after;
                jpp::array records = jpp::make_array();

                {
                    MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );

                    for( const jpp::object & record : m_logs )
                    {
                        uint64_t sequence = record.get( "sequence", MENGINE_UINT64_C(0) );

                        if( sequence <= after )
                        {
                            continue;
                        }

                        records.push_back( record );
                        next = sequence;

                        if( records.size() >= limit )
                        {
                            break;
                        }
                    }
                }

                jpp::object result = jpp::make_object();
                result.set( "records", records );
                result.set( "next", next );
                jpp::object command = jpp::make_object();
                command.set( "kind", "logsResult" );
                command.set( "id", id );
                command.set( "result", result );
                platform->postCommand( command );
            }break;
        case EAndroidWebViewEvent::CAPTURE:
            {
                String requestId = Helper::AndroidGetJavaJSONObjectString( _env, jsonClass, _event, "id", "" );
                platform->takeSnapshot( [platform, requestId]( const Data & _data, uint32_t _width, uint32_t _height )
                {
                    String encoded;
                    Helper::encodeBase64( _data, &encoded );
                    jpp::object result = jpp::make_object();
                    result.set( "kind", "captureResult" );
                    result.set( "id", requestId );
                    result.set( "width", _width );
                    result.set( "height", _height );
                    result.set( "data", encoded );
                    platform->postCommand( result );
                } );
            }break;
#endif
        case EAndroidWebViewEvent::COMPLETE_REQUEST:
            {
                String generationString = Helper::AndroidGetJavaJSONObjectString( _env, jsonClass, _event, "generation", "" );
                uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
                    ->getDocumentGeneration();

                String expectedGeneration = Helper::stringUInt64( generation );

                if( generationString != expectedGeneration )
                {
                    break;
                }

                String id = Helper::AndroidGetJavaJSONObjectString( _env, jsonClass, _event, "id", "" );
                String error = Helper::AndroidGetJavaJSONObjectString( _env, jsonClass, _event, "error", "" );
                jpp::object result = jpp::make_null();

                if( error.empty() == true )
                {
                    jobject javaResult = Helper::AndroidGetJavaJSONObjectElement( _env, jsonClass, _event, "result" );
                    bool successful = Helper::AndroidMakeJSONFromJObject( _env, javaResult, &result );
                    Mengine_JNI_DeleteLocalRef( _env, javaResult );

                    if( successful == false )
                    {
                        LOGGER_ERROR( "invalid WebView request result" );

                        break;
                    }
                }

                WEBVIEW_BRIDGE_SERVICE()
                    ->completeRequest( generation, id, result, error );
            }break;
        case EAndroidWebViewEvent::MEMORY:
            {
                int32_t level = Helper::AndroidGetJavaJSONObjectInt( _env, jsonClass, _event, "level", -1 );

                if( level < 0 )
                {
                    LIFECYCLE_SERVICE()
                        ->onApplicationDidReceiveMemoryWarning();
                }
                else
                {
                    LIFECYCLE_SERVICE()
                        ->onApplicationDidReceiveTrimMemory( level );
                }
            }break;
        case EAndroidWebViewEvent::ACTIVITY_START:
            {
                LIFECYCLE_SERVICE()
                    ->onApplicationWillEnterForeground();
            }break;
        case EAndroidWebViewEvent::ACTIVITY_STOP:
            {
                LIFECYCLE_SERVICE()
                    ->onApplicationDidEnterBackground();
            }break;
        case EAndroidWebViewEvent::ACTIVITY_RESUME:
            {
                LIFECYCLE_SERVICE()
                    ->onApplicationDidBecomeActive();
            }break;
        case EAndroidWebViewEvent::ACTIVITY_PAUSE:
            {
                LIFECYCLE_SERVICE()
                    ->onApplicationWillResignActive();
            }break;
        default:
            {
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewApplication::registerApplicationHandlers_()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        jbyteArray bytes = (jbyteArray)Helper::AndroidCallObjectPluginMethod( env, "AndroidWebViewPlugin", "getWebViewHandlerNames", "()[B" );

        if( bytes == nullptr )
        {
            return false;
        }

        jsize size = env->GetArrayLength( bytes );
        jbyte * buffer = env->GetByteArrayElements( bytes, nullptr );
        size_t bufferSize = static_cast<size_t>(size);
        jpp::object names = Helper::loadJSONBuffer( buffer, bufferSize, nullptr );
        env->ReleaseByteArrayElements( bytes, buffer, JNI_ABORT );
        env->DeleteLocalRef( bytes );

        if( names.is_type_array() == false )
        {
            return false;
        }

        jpp::array handlers = names;

        for( const jpp::object & value : handlers )
        {
            if( value.is_type_string() == false )
            {
                return false;
            }

            String name = value;
            bool added = WEBVIEW_BRIDGE_SERVICE()
                ->addHandler( name, [name]( uint64_t _generation, const String & _id, const jpp::object & _arguments )
                {
                    jpp::object command = jpp::make_object();
                    command.set( "kind", "request" );
                    command.set( "method", name );
                    String generation = Helper::stringUInt64( _generation );
                    command.set( "generation", generation );
                    command.set( "id", _id );
                    command.set( "arguments", _arguments );
                    AndroidWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
                        ->getUnknown();
                    platform->postCommand( command );
                } );

            if( added == false )
            {
                return false;
            }
        }

        WebViewPlatformExtensionInterface * browser = WEBVIEW_PLATFORM_SERVICE()
            ->getUnknown();
        browser->setExternalNavigationCallback( []( const String & _url )
        {
            jpp::object command = jpp::make_object();
            command.set( "kind", "external" );
            command.set( "url", _url );
            AndroidWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
                ->getUnknown();
            platform->postCommand( command );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewApplication::publishStatus_()
    {
        bool ready = WEBVIEW_BRIDGE_SERVICE()
            ->isDocumentReady();
        bool appReady = WEBVIEW_BRIDGE_SERVICE()
            ->isAppReady();
        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();
        jpp::object status = jpp::make_object();
        status.set( "kind", "status" );
        status.set( "documentReady", ready );
        status.set( "appReady", appReady );
        String generationString = Helper::stringUInt64( generation );
        status.set( "generation", generationString );
        status.set( "runtimeTimer", true );
        AndroidWebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
            ->getUnknown();
        platform->postCommand( status );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewApplication::finalize()
    {
        if( SERVICE_PROVIDER_EXIST() == false )
        {
            return;
        }

        if( SERVICE_IS_INITIALIZE( LifecycleServiceInterface ) == true )
        {
            LIFECYCLE_SERVICE()
                ->onApplicationWillTerminate();
        }

        if( SERVICE_IS_INITIALIZE( WebViewBootstrapperInterface ) == true )
        {
            WEBVIEW_BOOTSTRAPPER_SERVICE()
                ->stop();
        }

        SERVICE_FINALIZE( WebViewPlatformService );
        SERVICE_DESTROY( WebViewPlatformService );

        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == true )
        {
            if( FILE_SERVICE()
                ->hasFileGroup( STRINGIZE_STRING_LOCAL( "webview" ), nullptr ) == true )
            {
                FILE_SERVICE()
                    ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "webview" ) );
            }

            if( FILE_SERVICE()
                ->hasFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ), nullptr ) == true )
            {
                FILE_SERVICE()
                    ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "temporary" ) );
            }
        }

        SERVICE_FINALIZE( EnvironmentService );
        SERVICE_DESTROY( EnvironmentService );
        SERVICE_FINALIZE( AndroidAssetService );
        SERVICE_DESTROY( AndroidAssetService );
        SERVICE_FINALIZE( AndroidKernelService );
        SERVICE_DESTROY( AndroidKernelService );

#if defined(MENGINE_WEBVIEW_MCP)
        if( m_logCollector != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_logCollector );
            m_logCollector = nullptr;
        }

        m_logs = jpp::array::none();
        m_logMutex = nullptr;
#endif

        if( m_logger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_logger );
            m_logger = nullptr;
        }

        API_MengineFinalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
