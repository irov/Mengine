#include "AndroidEnvironmentService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Android/AndroidUtils.h"

#include "AndroidProxyLogger.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static jclass g_jclass_MengineActivity;
static jobject g_jobject_MengineActivity;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1setMengineAndroidActivityJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_MengineActivity = (jclass)env->NewGlobalRef( cls );
        g_jobject_MengineActivity = (jclass)env->NewGlobalRef( obj );
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
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getConfigValue )(JNIEnv * env, jclass cls, jstring _section, jstring _key, jstring _default)
    {
        const Mengine::ConfigInterfacePtr & defaultConfig = CONFIG_SERVICE()
            ->getDefaultConfig();

        const Mengine::Char * section_str = env->GetStringUTFChars( _section, nullptr );
        const Mengine::Char * key_str = env->GetStringUTFChars( _key, nullptr );
        const Mengine::Char * default_str = env->GetStringUTFChars( _default, nullptr );

        const Mengine::Char * value_str = nullptr;
        bool result = defaultConfig->hasValue( section_str, key_str, default_str, &value_str );

        env->ReleaseStringUTFChars( _section, section_str );
        env->ReleaseStringUTFChars( _key, key_str );
        env->ReleaseStringUTFChars( _default, default_str );

        if( result == false )
        {
            return _default;
        }

        jstring value = env->NewStringUTF( value_str );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getConfigValueBoolean )(JNIEnv * env, jclass cls, jstring _section, jstring _key, jboolean _default)
    {
        const Mengine::ConfigInterfacePtr & defaultConfig = CONFIG_SERVICE()
            ->getDefaultConfig();

        const Mengine::Char * section_str = env->GetStringUTFChars( _section, nullptr );
        const Mengine::Char * key_str = env->GetStringUTFChars( _key, nullptr );

        bool value_boolean;
        bool result = defaultConfig->hasValue( section_str, key_str, _default, &value_boolean );

        env->ReleaseStringUTFChars( _section, section_str );
        env->ReleaseStringUTFChars( _key, key_str );

        if( result == false )
        {
            return _default;
        }

        return value_boolean;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1hasOption )(JNIEnv * env, jclass cls, jstring _option)
    {
        const Mengine::ConfigInterfacePtr & defaultConfig = CONFIG_SERVICE()
            ->getDefaultConfig();

        const Mengine::Char * option_str = env->GetStringUTFChars( _option, nullptr );

        bool result = OPTIONS_SERVICE()
            ->hasOption( option_str, false );

        env->ReleaseStringUTFChars( _option, option_str );

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
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AndroidEnvironmentService, Mengine::AndroidEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidEnvironmentService::AndroidEnvironmentService()
        : m_jclass_Object( nullptr )
        , m_jclass_Boolean( nullptr )
        , m_jclass_Character( nullptr )
        , m_jclass_Integer( nullptr )
        , m_jclass_Long( nullptr )
        , m_jclass_Float( nullptr )
        , m_jclass_Double( nullptr )
        , m_jclass_String( nullptr )
        , m_jclass_ArrayList( nullptr )
        , m_jclass_Map( nullptr )
        , m_jclass_HashMap( nullptr )
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

        m_jclass_Object = jenv->FindClass( "java/lang/Object" );
        m_jclass_Boolean = jenv->FindClass( "java/lang/Boolean" );
        m_jclass_Character = jenv->FindClass( "java/lang/Character" );
        m_jclass_Integer = jenv->FindClass( "java/lang/Integer" );
        m_jclass_Long = jenv->FindClass( "java/lang/Long" );
        m_jclass_Float = jenv->FindClass( "java/lang/Float" );
        m_jclass_Double = jenv->FindClass( "java/lang/Double" );
        m_jclass_String = jenv->FindClass( "java/lang/String" );
        m_jclass_ArrayList = jenv->FindClass( "java/util/ArrayList" );
        m_jclass_Map = jenv->FindClass( "java/util/Map" );
        m_jclass_HashMap = jenv->FindClass( "java/util/HashMap" );

        m_androidEventationHub = Helper::makeFactorableUnique<AndroidEventationHub>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_androidEventationHub->initialize() == false )
        {
            return false;
        }

        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        proxyLogger->setJClassMengineActivity( g_jclass_MengineActivity );
        proxyLogger->setJObjectMengineActivity( g_jobject_MengineActivity );

        LOGGER_SERVICE()
            ->registerLogger( proxyLogger );

        m_proxyLogger = proxyLogger;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_RUN, &AndroidEnvironmentService::notifyApplicationRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_READY, &AndroidEnvironmentService::notifyApplicationReady_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_STOP, &AndroidEnvironmentService::notifyApplicationStop_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AndroidEnvironmentService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );

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

        LOGGER_SERVICE()
            ->unregisterLogger( m_proxyLogger );

        m_proxyLogger = nullptr;

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        m_androidEventationHub->finalize();
        m_androidEventationHub = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openUrlInDefaultBrowser(const Char * _url )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        bool result = Mengine::Helper::AndroidOpenUrlInDefaultBrowser( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _url );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openMail(const Char * _email, const Char * _subject, const Char * _body )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        bool result = Mengine::Helper::AndroidOpenMail( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _email, _subject, _body );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassMengineActivity() const
    {
        return g_jclass_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::getJObjectMengineActivity() const
    {
        return g_jobject_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassObject() const
    {
        return  m_jclass_Object;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassBoolean() const
    {
        return  m_jclass_Boolean;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassCharacter() const
    {
        return  m_jclass_Character;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassInteger() const
    {
        return  m_jclass_Integer;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassLong() const
    {
        return  m_jclass_Long;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassFloat() const
    {
        return  m_jclass_Float;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassDouble() const
    {
        return  m_jclass_Double;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassString() const
    {
        return  m_jclass_String;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassArrayList() const
    {
        return  m_jclass_ArrayList;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassMap() const
    {
        return  m_jclass_Map;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClassHashMap() const
    {
        return  m_jclass_HashMap;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidId(Char * _androidId, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_getAndroidId = jenv->GetMethodID( g_jclass_MengineActivity, "getAndroidId", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getAndroidId != nullptr, "invalid get android method 'getAndroidId'" );

        jstring jReturnValue = (jstring)jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getAndroidId );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _androidId, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceName(Char * _deviceName, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_getDeviceName = jenv->GetMethodID( g_jclass_MengineActivity, "getDeviceName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getDeviceName != nullptr, "invalid get android method 'getDeviceName'" );

        jstring jReturnValue = (jstring)jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getDeviceName );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidPackageName(Char * _packageName, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_getPackageName = jenv->GetMethodID( g_jclass_MengineActivity, "getPackageName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getPackageName != nullptr, "invalid get android method 'getPackageName'" );

        jstring jReturnValue = (jstring)jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getPackageName );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _packageName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidOpenAssetFile(const Char * _path )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t fileId = Helper::AndroidOpenAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidAvailableAssetFile(int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t available = Helper::AndroidAvailableAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidReadAssetFile(int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t read = Helper::AndroidReadAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidSkipAssetFile(int32_t _fileId, int32_t _offset )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        int32_t skip = Helper::AndroidSkipAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidResetAssetFile(int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Helper::AndroidResetAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidCloseAssetFile(int32_t _fileId )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Helper::AndroidCloseAssetFile( jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::addAndroidEventation(const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->addAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::removeAndroidEventation(const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->removeAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::onAnalyticsEvent(const AnalyticsEventInterfacePtr & _event )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        jobject eventName_jobject = this->makeJObjectString( jenv, eventName_str );

        TimeMilliseconds eventTimestamp = _event->getTimestamp();

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = this->makeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [this, parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
           {
               const Char * name_str = _name.c_str();

               jobject name_jvalue = this->makeJObjectString( jenv, name_str );

               EAnalyticsEventParameterType parameterType = _parameter->getType();

               jobject parameter_jobject = nullptr;

               switch( parameterType )
               {
               case EAEPT_BOOLEAN:
                   {
                       AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                       bool parameter_value = parameter_boolean->resolveValue();

                       parameter_jobject = this->makeJObjectBoolean( jenv, parameter_value );
                   }break;
               case EAEPT_INTEGER:
                   {
                       AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                       int64_t parameter_value = parameter_integer->resolveValue();

                       parameter_jobject = this->makeJObjectLong( jenv, parameter_value );
                   }break;
               case EAEPT_DOUBLE:
                   {
                       AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                       double parameter_value = parameter_double->resolveValue();

                       parameter_jobject = this->makeJObjectDouble( jenv, parameter_value );
                   }break;
               case EAEPT_STRING:
                   {
                       AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                       const String & parameter_value = parameter_string->resolveValue();

                       const Char * parameter_value_str = parameter_value.c_str();

                       parameter_jobject = this->makeJObjectString( jenv, parameter_value_str );
                   }break;
               }

               static jmethodID Map_put = jenv->GetMethodID( m_jclass_Map, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" );

               jobject result_jobject = jenv->CallObjectMethod( parameters_jobject, Map_put, name_jvalue, parameter_jobject );

               jenv->DeleteLocalRef( result_jobject );

               jenv->DeleteLocalRef( parameter_jobject );
           });

        static jmethodID jmethodID_onMengineAnalyticsEvent = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineAnalyticsEvent", "(Ljava/lang/String;JLjava/util/Map;)V" );

        MENGINE_ASSERTION( jmethodID_onMengineAnalyticsEvent != nullptr, "invalid get android method 'onMengineAnalyticsEvent'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineAnalyticsEvent, eventName_jobject, (jlong)eventTimestamp, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef( parameters_jobject );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationRun_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineApplicationRun = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationRun", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationRun != nullptr, "invalid get android method 'onMengineApplicationRun'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationRun );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationReady_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineApplicationReady = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationReady", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationReady != nullptr, "invalid get android method 'onMengineApplicationReady'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationReady );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationStop_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineApplicationStop = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationStop", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationStop != nullptr, "invalid get android method 'onMengineApplicationStop'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationStop );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineInitializeBaseServices = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineInitializeBaseServices", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineInitializeBaseServices != nullptr, "invalid get android method 'onMengineInitializeBaseServices'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineInitializeBaseServices );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperCreateApplication_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineCreateApplication = jenv->GetMethodID( g_jclass_MengineActivity, "onMengineCreateApplication", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineCreateApplication != nullptr, "invalid get android method 'onMengineCreateApplication'" );

        jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineCreateApplication );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectBoolean( JNIEnv * _jenv, bool _value )
    {
        static jmethodID Boolean_constructor = _jenv->GetMethodID( m_jclass_Boolean, "<init>", "(Z)V" );

        MENGINE_ASSERTION( Boolean_constructor != nullptr, "invalid get android method 'Boolean <init> (Z)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_Boolean, Boolean_constructor, _value );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Boolean '%s'"
            , _value == true ? "true" : "false"
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectInteger( JNIEnv * _jenv, int32_t _value )
    {
        static jmethodID Integer_constructor = _jenv->GetMethodID( m_jclass_Integer, "<init>", "(I)V" );

        MENGINE_ASSERTION( Integer_constructor != nullptr, "invalid get android method 'Integer <init> (I)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_Integer, Integer_constructor, _value );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Integer '%d'"
            , _value
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectLong( JNIEnv * _jenv, int64_t _value )
    {
        static jmethodID Long_constructor = _jenv->GetMethodID( m_jclass_Long, "<init>", "(J)V" );

        MENGINE_ASSERTION( Long_constructor != nullptr, "invalid get android method 'Long <init> (J)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_Long, Long_constructor, _value );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Long '%ld'"
            , _value
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectFloat( JNIEnv * _jenv, float _value )
    {
        static jmethodID Float_constructor = _jenv->GetMethodID( m_jclass_Float, "<init>", "(F)V" );

        MENGINE_ASSERTION( Float_constructor != nullptr, "invalid get android method 'Float <init> (F)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_Float, Float_constructor, _value );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Float '%f'"
            , _value
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectDouble( JNIEnv * _jenv, double _value )
    {
        static jmethodID Double_constructor = _jenv->GetMethodID( m_jclass_Double, "<init>", "(D)V" );

        MENGINE_ASSERTION( Double_constructor != nullptr, "invalid get android method 'Double <init> (D)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_Double, Double_constructor, _value );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create Double '%lf'"
            , _value
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectString( JNIEnv * _jenv, const Char * _value )
    {
        static jmethodID String_constructor = _jenv->GetMethodID( m_jclass_String, "<init>", "(Ljava/lang/String;)V" );

        MENGINE_ASSERTION( String_constructor != nullptr, "invalid get android method 'String <init> (Ljava/lang/String;)V'" );

        jstring value_jstring = _jenv->NewStringUTF( _value );

        jobject value_jobject = _jenv->NewObject( m_jclass_String, String_constructor, value_jstring );

        _jenv->DeleteLocalRef( value_jstring );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create String '%s'"
            , _value
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectHashMap( JNIEnv * _jenv, int32_t _count )
    {
        static jmethodID HashMap_constructor = _jenv->GetMethodID( m_jclass_HashMap, "<init>", "(I)V" );

        MENGINE_ASSERTION( HashMap_constructor != nullptr, "invalid get android method 'HashMap <init> (I)V'" );

        jobject value_jobject = _jenv->NewObject( m_jclass_HashMap, HashMap_constructor, _count );

        MENGINE_ASSERTION( value_jobject != nullptr, "invalid create HashMap '%d'"
            , _count
        );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
}
