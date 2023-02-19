#include "AndroidEnvironmentService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidAssetFile.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidUtils.h"

#include "AndroidProxyLogger.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

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
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1logInfo )(JNIEnv * env, jclass cls, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1logWarning )(JNIEnv * env, jclass cls, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_WARNING, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1logMessage )(JNIEnv * env, jclass cls, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1logMessageRelease )(JNIEnv * env, jclass cls, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1logError )(JNIEnv * env, jclass cls, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::LoggerServiceInterface) == false )
        {
            return;
        }

        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
        );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICS_JAVA_INTERFACE( AndroidAnalyticsService_1addContextParameterBoolean )(JNIEnv * env, jclass cls, jstring _name, jboolean _value)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::AnalyticsServiceInterface) == false )
        {
            return;
        }

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        const Mengine::AnalyticsContextInterfacePtr & context = ANALYTICS_SERVICE()
                ->getGlobalContext();

        context->addParameterBoolean( name_cstr, (bool)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICS_JAVA_INTERFACE( AndroidAnalyticsService_1addContextParameterString )(JNIEnv * env, jclass cls, jstring _name, jstring _value)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::AnalyticsServiceInterface) == false )
        {
            return;
        }

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );
        Mengine::String value_string = Mengine::Helper::makeStringFromJString( env, _value );

        const Mengine::AnalyticsContextInterfacePtr & context = ANALYTICS_SERVICE()
            ->getGlobalContext();

        context->addParameterString( name_cstr, value_string );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICS_JAVA_INTERFACE( AndroidAnalyticsService_1addContextParameterInteger )(JNIEnv * env, jclass cls, jstring _name, jlong _value)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::AnalyticsServiceInterface) == false )
        {
            return;
        }

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        const Mengine::AnalyticsContextInterfacePtr & context = ANALYTICS_SERVICE()
                ->getGlobalContext();

        context->addParameterInteger( name_cstr, (int64_t)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICS_JAVA_INTERFACE( AndroidAnalyticsService_1addContextParameterDouble )(JNIEnv * env, jclass cls, jstring _name, jdouble _value)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::AnalyticsServiceInterface) == false )
        {
            return;
        }

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        const Mengine::AnalyticsContextInterfacePtr & context = ANALYTICS_SERVICE()
                ->getGlobalContext();

        context->addParameterDouble( name_cstr, (double)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jobject JNICALL MENGINE_ANALYTICS_JAVA_INTERFACE( AndroidAnalyticsService_1buildEvent )(JNIEnv * env, jclass cls, jstring _name)
    {
        if( SERVICE_IS_INITIALIZE(Mengine::AnalyticsServiceInterface) == false )
        {
            return nullptr;
        }

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        Mengine::AnalyticsEventBuilderInterfacePtr analyticsEventBuilder = ANALYTICS_SERVICE()
            ->buildEvent( name_cstr );

        Mengine::Factorable * factorable = analyticsEventBuilder.getT<Mengine::Factorable *>();

        Mengine::Factorable::intrusive_ptr_add_ref( factorable );

        jobject jptr = env->NewDirectByteBuffer( factorable, 0 );

        return jptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE( AndroidAnalyticsService_1addEventBuilderParameterBoolean )(JNIEnv * env, jclass cls, jobject _jptr, jstring _name, jboolean _value)
    {
        void * ptr = env->GetDirectBufferAddress( _jptr );
        Mengine::Factorable * factorable = reinterpret_cast<Mengine::Factorable *>( ptr );

        Mengine::AnalyticsEventBuilderInterface * eventBuilder = static_cast<Mengine::AnalyticsEventBuilderInterface *>(factorable);

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        eventBuilder->addParameterBoolean( name_cstr, (bool)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE( AndroidAnalyticsService_1addEventBuilderParameterString )(JNIEnv * env, jclass cls, jobject _jptr, jstring _name, jstring _value)
    {
        void * ptr = env->GetDirectBufferAddress( _jptr );
        Mengine::Factorable * factorable = reinterpret_cast<Mengine::Factorable *>( ptr );

        Mengine::AnalyticsEventBuilderInterface * eventBuilder = static_cast<Mengine::AnalyticsEventBuilderInterface *>(factorable);

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );
        Mengine::String value_string = Mengine::Helper::makeStringFromJString( env, _value );

        eventBuilder->addParameterString( name_cstr, value_string );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE( AndroidAnalyticsService_1addEventBuilderParameterInteger )(JNIEnv * env, jclass cls, jobject _jptr, jstring _name, jlong _value)
    {
        void * ptr = env->GetDirectBufferAddress( _jptr );
        Mengine::Factorable * factorable = reinterpret_cast<Mengine::Factorable *>( ptr );

        Mengine::AnalyticsEventBuilderInterface * eventBuilder = static_cast<Mengine::AnalyticsEventBuilderInterface *>(factorable);

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        eventBuilder->addParameterInteger( name_cstr, (int64_t)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE( AndroidAnalyticsService_1addEventBuilderParameterDouble )(JNIEnv * env, jclass cls, jobject _jptr, jstring _name, jdouble _value)
    {
        void * ptr = env->GetDirectBufferAddress( _jptr );
        Mengine::Factorable * factorable = reinterpret_cast<Mengine::Factorable *>( ptr );

        Mengine::AnalyticsEventBuilderInterface * eventBuilder = static_cast<Mengine::AnalyticsEventBuilderInterface *>(factorable);

        Mengine::ConstString name_cstr = Mengine::Helper::makeConstStringFromJString( env, _name );

        eventBuilder->addParameterDouble( name_cstr, (double)_value );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE( AndroidAnalyticsService_1logEventBuilder )(JNIEnv * env, jclass cls, jobject _jptr)
    {
        void * ptr = env->GetDirectBufferAddress( _jptr );
        Mengine::Factorable * factorable = reinterpret_cast<Mengine::Factorable *>( ptr );

        Mengine::AnalyticsEventBuilderInterface * eventBuilder = static_cast<Mengine::AnalyticsEventBuilderInterface *>(factorable);

        eventBuilder->log();

        Mengine::Factorable::intrusive_ptr_dec_ref( factorable );
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
        JNIEnv * jenv = Mengine_JNI_GetEnv();

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

        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_RUN, &AndroidEnvironmentService::notifyApplicationRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_READY, &AndroidEnvironmentService::notifyApplicationReady_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_STOP, &AndroidEnvironmentService::notifyApplicationStop_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AndroidEnvironmentService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &AndroidEnvironmentService::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_END_UPDATE, &AndroidEnvironmentService::notifyApplicationEndUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_RUN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_READY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_STOP );
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        bool result = Mengine::Helper::AndroidOpenUrlInDefaultBrowser( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _url );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

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
    int32_t AndroidEnvironmentService::androidOpenAssetFile( const Char * _path )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t fileId = Helper::AndroidOpenAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidAvailableAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t available = Helper::AndroidAvailableAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t read = Helper::AndroidReadAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidSkipAssetFile( int32_t _fileId, int32_t _offset )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t skip = Helper::AndroidSkipAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidResetAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Helper::AndroidResetAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidCloseAssetFile( int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

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

        EAnalyticsEventType eventType = _event->getType();

        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        jobject eventName_jobject = Helper::makeJObjectString( jenv, eventName_str );

        TimeMilliseconds eventTimestamp = _event->getTimestamp();

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = Helper::makeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [this, parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
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

        jmethodID jmethodID_onMengineAnalyticsEvent = this->getApplicationMethodID( jenv, "onMengineAnalyticsEvent", "(ILjava/lang/String;JLjava/util/Map;)V" );

        this->callVoidApplicationMethod( jenv, jmethodID_onMengineAnalyticsEvent, (jint)eventType, eventName_jobject, (jlong)eventTimestamp, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef( parameters_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationRun_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_onMengineApplicationRun = this->getActivityMethodID( jenv, "onMengineApplicationRun", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineApplicationRun );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationReady_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_onMengineApplicationReady = this->getActivityMethodID( jenv, "onMengineApplicationReady", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineApplicationReady );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationStop_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_onMengineApplicationStop = this->getActivityMethodID( jenv, "onMengineApplicationStop", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineApplicationStop );

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

        jmethodID jmethodID_onMengineInitializeBaseServices = this->getActivityMethodID( jenv, "onMengineInitializeBaseServices", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineInitializeBaseServices );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperCreateApplication_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_onMengineCreateApplication = this->getActivityMethodID( jenv, "onMengineCreateApplication", "()V" );

        this->callVoidActivityMethod( jenv, jmethodID_onMengineCreateApplication );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}