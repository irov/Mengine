#include "AndroidEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidAssetFile.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidUtils.h"

#include "AndroidProxyLogger.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ProxyLogger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
static jclass g_jclass_MengineApplication;
static jobject g_jobject_MengineApplication;
//////////////////////////////////////////////////////////////////////////
static jclass g_jclass_MengineActivity;
static jobject g_jobject_MengineActivity;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_APPLICATION_JAVA_INTERFACE( AndroidEnvironmentService_1setMengineAndroidApplicationJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_MengineApplication = (jclass)env->NewGlobalRef( cls );
        g_jobject_MengineApplication = (jclass)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_APPLICATION_JAVA_INTERFACE( AndroidEnvironmentService_1removeMengineAndroidApplicationJNI )(JNIEnv * env, jclass cls)
    {
        env->DeleteGlobalRef( g_jclass_MengineApplication );
        g_jclass_MengineApplication = nullptr;

        env->DeleteGlobalRef( g_jobject_MengineApplication );
        g_jobject_MengineApplication = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1setMengineAndroidActivityJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_MengineActivity = (jclass)env->NewGlobalRef( cls );
        g_jobject_MengineActivity = (jclass)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1removeMengineAndroidActivityJNI )(JNIEnv * env, jclass cls)
    {
        env->DeleteGlobalRef( g_jclass_MengineActivity );
        g_jclass_MengineActivity = nullptr;

        env->DeleteGlobalRef( g_jobject_MengineActivity );
        g_jobject_MengineActivity = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1quitMengineAndroidActivityJNI )(JNIEnv * env, jclass cls)
    {
        PLATFORM_SERVICE()
            ->closeWindow();
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getCompanyName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        jstring result = env->NewStringUTF( companyName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getProjectName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        jstring result = env->NewStringUTF( projectName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getExtraPreferencesFolderName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char extraPreferencesFolderName[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getExtraPreferencesFolderName( extraPreferencesFolderName );

        jstring result = env->NewStringUTF( extraPreferencesFolderName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1hasCurrentAccount )(JNIEnv * env, jclass cls)
    {
        bool result = ACCOUNT_SERVICE()
            ->hasCurrentAccount();

        return (jboolean)result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getCurrentAccountFolderName )(JNIEnv * env, jclass cls)
    {
        const Mengine::AccountInterfacePtr & account = ACCOUNT_SERVICE()
            ->getCurrentAccount();

        if (account == nullptr) {
            return env->NewStringUTF( "" );
        }

        const Mengine::ConstString & folderName = account->getFolderName();

        jstring result = env->NewStringUTF( folderName.c_str() );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1writeLoggerHistoryToFile )(JNIEnv * env, jclass cls, jobject _writer)
    {
        Mengine::ProxyLoggerPtr proxyLog = Mengine::Helper::makeFactorableUnique<Mengine::ProxyLogger>( MENGINE_DOCUMENT_FUNCTION );

        if( proxyLog->initializeLogger() == false )
        {
            return JNI_FALSE;
        }

        proxyLog->setLambda([env, _writer]( const Mengine::LoggerMessage & _message ) {
            jclass jclass_Writer = env->GetObjectClass( _writer );
            jmethodID jmethodID_Writer_write_String = env->GetMethodID( jclass_Writer, "write", "(Ljava/lang/String;)V" );
            jmethodID jmethodID_Writer_write_Char = env->GetMethodID( jclass_Writer, "write", "(I)V" );

            if( _message.flag & Mengine::LFLAG_FUNCTIONSTAMP )
            {
                Mengine::Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
                Mengine::Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

                jstring jvalue = env->NewStringUTF( functionstamp );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ' ' );
                env->DeleteLocalRef( jvalue );
            }

            if( _message.flag & Mengine::LFLAG_TIMESTAMP )
            {
                Mengine::Char timestamp[256] = {'\0'};
                Mengine::Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );

                jstring jvalue = env->NewStringUTF( timestamp );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ' ' );
                env->DeleteLocalRef( jvalue );
            }

            if( _message.flag & Mengine::LFLAG_THREADSTAMP )
            {
                Mengine::Char threadstamp[256] = {'\0'};
                Mengine::Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );

                jstring jvalue = env->NewStringUTF( threadstamp );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ' ' );
                env->DeleteLocalRef( jvalue );
            }

            if( _message.flag & Mengine::LFLAG_SYMBOLSTAMP )
            {
                Mengine::ELoggerLevel level = _message.level;

                Mengine::Char symbol = Mengine::Helper::getLoggerLevelSymbol( level );

                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, symbol );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ' ' );
            }

            if( _message.flag & Mengine::LFLAG_CATEGORYSTAMP )
            {
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, '[' );
                jstring jvalue = env->NewStringUTF( _message.category );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
                env->DeleteLocalRef( jvalue );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ']' );
                env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, ' ' );
            }

            const Mengine::Char * data_value = _message.data;
            size_t data_size = _message.size;

            Mengine::Char msg[MENGINE_LOGGER_MAX_MESSAGE];
            MENGINE_MEMCPY( msg, data_value, data_size * sizeof(Mengine::Char) );
            msg[data_size] = '\0';

            jstring jvalue = env->NewStringUTF( msg );
            env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
            env->DeleteLocalRef( jvalue );

            env->CallVoidMethod( _writer, jmethodID_Writer_write_Char, '\n' );

            env->DeleteLocalRef( jclass_Writer );
        });

        LOGGER_SERVICE()
            ->writeHistory( proxyLog );

        proxyLog->flush();
        proxyLog = nullptr;

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getProjectVersion )(JNIEnv * env, jclass cls)
    {
        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        jint result = (jint)projectVersion;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1isDebugMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDebugMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1isDevelopmentMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDevelopmentMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1isMasterRelease )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isMasterRelease();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1isBuildPublish )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isBuildPublish();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getEngineGITSHA1 )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * ENGINE_GIT_SHA1 = Mengine::Helper::getEngineGITSHA1();

        jstring result = env->NewStringUTF( ENGINE_GIT_SHA1 );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getBuildTimestamp )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_TIMESTAMP = Mengine::Helper::getBuildTimestamp();

        jstring result = env->NewStringUTF( BUILD_TIMESTAMP );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getBuildUsername )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_USERNAME = Mengine::Helper::getBuildUsername();

        jstring result = env->NewStringUTF( BUILD_USERNAME );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getBuildVersion )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();

        jstring result = env->NewStringUTF( BUILD_VERSION );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1log )(JNIEnv * env, jclass cls, jint _level, jstring _tag, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * tag_str = env->GetStringUTFChars( _tag, nullptr );
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        Mengine::ELoggerLevel level;
        uint32_t color;
        switch (_level) {
            case 0:
                return;
            case 1:
                level = Mengine::LM_FATAL;
                color = Mengine::LCOLOR_RED;
                break;
            case 2:
                level = Mengine::LM_MESSAGE_RELEASE;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE;
                break;
            case 3:
                level = Mengine::LM_ERROR;
                color = Mengine::LCOLOR_RED;
                break;
            case 4:
                level = Mengine::LM_WARNING;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN;
                break;
            case 5:
                level = Mengine::LM_MESSAGE;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE;
                break;
            case 6:
                level = Mengine::LM_INFO;
                color = Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE;
                break;
            case 7:
                level = Mengine::LM_DEBUG;
                color = Mengine::LCOLOR_BLUE;
                break;
            case 8:
                return;
                break;
        }

        LOGGER_VERBOSE_LEVEL( "android", level, Mengine::LFILTER_NONE, color, nullptr, 0, Mengine::LFLAG_SHORT )("[%s] %s"
            , tag_str
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, tag_str );
        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidEnvironmentService, Mengine::AndroidEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidEnvironmentService::AndroidEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidEnvironmentService::~AndroidEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::_initializeService()
    {
        AndroidEventationHubPtr androidEventationHub = Helper::makeFactorableUnique<AndroidEventationHub>( MENGINE_DOCUMENT_FACTORABLE );

        if( androidEventationHub->initialize() == false )
        {
            return false;
        }

        m_androidEventationHub = androidEventationHub;

        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        LOGGER_SERVICE()
            ->registerLogger( proxyLogger );

        m_proxyLogger = proxyLogger;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_RUN, &AndroidEnvironmentService::notifyPlatformRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_READY, &AndroidEnvironmentService::notifyPlatformReady_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_STOP, &AndroidEnvironmentService::notifyPlatformStop_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AndroidEnvironmentService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &AndroidEnvironmentService::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_END_UPDATE, &AndroidEnvironmentService::notifyApplicationEndUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_RUN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_READY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_STOP );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_END_UPDATE );

        LOGGER_SERVICE()
            ->unregisterLogger( m_proxyLogger );

        m_proxyLogger = nullptr;

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        m_androidEventationHub->finalize();
        m_androidEventationHub = nullptr;

        for( IntrusiveListConstStringHolderJString::iterator
             it = m_holdersJString.begin();
             it != m_holdersJString.end(); )
        {
            IntrusiveListConstStringHolderJString::iterator it_erase = it;

            ConstStringHolderJString * holder = *it;
            ++it;

            m_holdersJString.erase( it_erase );

            m_poolJString.destroyT( holder );
        }

        m_holdersJString.clear();
        m_poolJString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return false;
        }

        bool result = Mengine::Helper::AndroidOpenUrlInDefaultBrowser( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _url );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return false;
        }

        bool result = Mengine::Helper::AndroidOpenMail( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _email, _subject, _body );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClass( JNIEnv * _jenv, const Char * _signature ) const
    {
        jclass clazz = _jenv->FindClass( _signature );

        return clazz;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID AndroidEnvironmentService::getActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const
    {
        jmethodID jmethodID_function = _jenv->GetMethodID( g_jclass_MengineActivity, _name, _signature );

        Helper::jEnvExceptionCheck( _jenv );

        MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android activity method '%s' with signature '%s'"
            , _name
            , _signature
        );

        return jmethodID_function;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::callVoidActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _method );

        _jenv->CallVoidMethodV( g_jobject_MengineActivity, _method, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::callObjectActivityMethod( JNIEnv * _jenv, jmethodID _method, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _method );

        jobject result_jobject = _jenv->CallObjectMethodV( g_jobject_MengineActivity, _method, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return result_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID AndroidEnvironmentService::getApplicationMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const
    {
        jmethodID jmethodID_function = _jenv->GetMethodID( g_jclass_MengineApplication, _name, _signature );

        Helper::jEnvExceptionCheck( _jenv );

        MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android application method '%s' with signature '%s'"
            , _name
            , _signature
        );

        return jmethodID_function;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::callVoidApplicationMethod( JNIEnv * _jenv, jmethodID _method, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _method );

        _jenv->CallVoidMethodV( g_jobject_MengineApplication, _method, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::callObjectApplicationMethod( JNIEnv * _jenv, jmethodID _method, ... ) const
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _method );

        jobject result_jobject = _jenv->CallObjectMethodV( g_jobject_MengineApplication, _method, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return result_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidId( Char * _androidId, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0;
        }

        jmethodID jmethodID_getAndroidId = this->getApplicationMethodID( jenv, "getAndroidId", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring)this->callObjectApplicationMethod( jenv, jmethodID_getAndroidId );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _androidId, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceName( Char * _deviceName, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        jmethodID jmethodID_getDeviceName = this->getApplicationMethodID( jenv, "getDeviceName", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring)this->callObjectApplicationMethod( jenv, jmethodID_getDeviceName );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceLanguage( Char * _deviceLanguage, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        jmethodID jmethodID_getDeviceLanguage = this->getApplicationMethodID( jenv, "getDeviceLanguage", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring) this->callObjectApplicationMethod(jenv, jmethodID_getDeviceLanguage );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceLanguage, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidPackageName( Char * _packageName, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        jmethodID jmethodID_getPackageName = this->getApplicationMethodID( jenv, "getPackageName", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring) this->callObjectApplicationMethod( jenv, jmethodID_getPackageName );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _packageName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr )
    {
        jsize value_length = _jenv->GetStringLength( _value );

        if( value_length == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        ConstStringHolderJString * holder = m_poolJString.createT();

        holder->setJString( _jenv, _value );

        if( STRINGIZE_SERVICE()
            ->stringizeExternal( holder, _cstr ) == false )
        {
            m_poolJString.destroyT( holder );

            return;
        }

        m_holdersJString.push_back( holder );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidOpenAssetFile( const Char * _path )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        int32_t fileId = Helper::AndroidOpenAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidAvailableAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        int32_t available = Helper::AndroidAvailableAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return 0;
        }

        int32_t read = Helper::AndroidReadAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidSkipAssetFile( int32_t _fileId, int32_t _offset )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return 0;
        }

        int32_t skip = Helper::AndroidSkipAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidResetAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        Helper::AndroidResetAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidCloseAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        Helper::AndroidCloseAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::addAndroidEventation( const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->addAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->removeAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::invokeAndroidEventations()
    {
        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        EAnalyticsEventType eventType = _event->getType();

        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        jobject eventName_jobject = Helper::makeJObjectString( jenv, eventName_str );

        Timestamp eventTimestamp = _event->getTimestamp();

        jobject context_jobject = Helper::makeJObjectHashMap( jenv, 0 );

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = Helper::makeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
           {
               const Char * name_str = _name.c_str();

               jobject name_jvalue = Helper::makeJObjectString( jenv, name_str );

               EAnalyticsEventParameterType parameterType = _parameter->getType();

               jobject parameter_jobject = nullptr;

               switch( parameterType )
               {
               case EAEPT_BOOLEAN:
                   {
                       AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                       bool parameter_value = parameter_boolean->resolveValue();

                       parameter_jobject = Helper::makeJObjectBoolean( jenv, parameter_value );
                   }break;
               case EAEPT_INTEGER:
                   {
                       AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                       int64_t parameter_value = parameter_integer->resolveValue();

                       parameter_jobject = Helper::makeJObjectLong( jenv, parameter_value );
                   }break;
               case EAEPT_DOUBLE:
                   {
                       AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                       double parameter_value = parameter_double->resolveValue();

                       parameter_jobject = Helper::makeJObjectDouble( jenv, parameter_value );
                   }break;
               case EAEPT_STRING:
                   {
                       AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                       const String & parameter_value = parameter_string->resolveValue();

                       const Char * parameter_value_str = parameter_value.c_str();

                       parameter_jobject = Helper::makeJObjectString( jenv, parameter_value_str );
                   }break;
               case EAEPT_CONSTSTRING:
                   {
                       AnalyticsEventParameterConstStringInterfacePtr parameter_string = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                       const ConstString & parameter_value = parameter_string->resolveValue();

                       const Char * parameter_value_str = parameter_value.c_str();

                       parameter_jobject = Helper::makeJObjectString( jenv, parameter_value_str );
                   }break;
               }

               jclass jclass_Map = jenv->FindClass( "java/util/Map" );

               jmethodID jmethodID_Map_put = jenv->GetMethodID( jclass_Map, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" );

               jobject result_jobject = jenv->CallObjectMethod( parameters_jobject, jmethodID_Map_put, name_jvalue, parameter_jobject );

               Helper::jEnvExceptionCheck( jenv );

               jenv->DeleteLocalRef( result_jobject );
               jenv->DeleteLocalRef( parameter_jobject );
               jenv->DeleteLocalRef( jclass_Map );
           });

        jmethodID jmethodID_onMengineAnalyticsEvent = this->getApplicationMethodID( jenv, "onMengineAnalyticsEvent", "(ILjava/lang/String;JLjava/util/Map;Ljava/util/Map;)V" );

        this->callVoidApplicationMethod( jenv, jmethodID_onMengineAnalyticsEvent, (jint)eventType, eventName_jobject, (jlong)eventTimestamp, context_jobject, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef( context_jobject );
        jenv->DeleteLocalRef( parameters_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyPlatformRun_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jmethodID jmethodID_onMenginePlatformRun = this->getActivityMethodID( jenv, "onMenginePlatformRun", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMenginePlatformRun );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyPlatformReady_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jmethodID jmethodID_onMenginePlatformReady = this->getActivityMethodID( jenv, "onMenginePlatformReady", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMenginePlatformReady );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyPlatformStop_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jmethodID jmethodID_onMenginePlatformStop = this->getActivityMethodID( jenv, "onMenginePlatformStop", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMenginePlatformStop );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationBeginUpdate_()
    {
        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationEndUpdate_()
    {
        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jmethodID jmethodID_onMengineInitializeBaseServices = this->getActivityMethodID( jenv, "onMengineInitializeBaseServices", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineInitializeBaseServices );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperCreateApplication_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL( "invalid get jenv" );

            return;
        }

        jmethodID jmethodID_onMengineCreateApplication = this->getActivityMethodID( jenv, "onMengineCreateApplication", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineCreateApplication );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}