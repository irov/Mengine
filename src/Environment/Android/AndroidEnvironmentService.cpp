#include "AndroidEnvironmentService.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Android/AndroidUtils.h"

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
            ->hasOption( option_str );

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
        : m_jenv( nullptr )
        , m_jclass_Object( nullptr )
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
        JNIEnv * env = Mengine_JNI_GetEnv();

        m_jenv = env;

        m_jclass_Object = m_jenv->FindClass( "java/lang/Object" );
        m_jclass_Boolean = m_jenv->FindClass( "java/lang/Boolean" );
        m_jclass_Character = m_jenv->FindClass( "java/lang/Character" );
        m_jclass_Integer = m_jenv->FindClass( "java/lang/Integer" );
        m_jclass_Long = m_jenv->FindClass( "java/lang/Long" );
        m_jclass_Float = m_jenv->FindClass( "java/lang/Float" );
        m_jclass_Double = m_jenv->FindClass( "java/lang/Double" );
        m_jclass_String = m_jenv->FindClass( "java/lang/String" );
        m_jclass_ArrayList = m_jenv->FindClass( "java/util/ArrayList" );
        m_jclass_Map = m_jenv->FindClass( "java/util/Map" );
        m_jclass_HashMap = m_jenv->FindClass( "java/util/HashMap" );

        m_androidEventationHub = Helper::makeFactorableUnique<AndroidEventationHub>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_androidEventationHub->initialize() == false )
        {
            return false;
        }

        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

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

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        m_androidEventationHub->finalize();
        m_androidEventationHub = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openUrlInDefaultBrowser(const Char * _url )
    {
        bool result = Mengine::Helper::AndroidOpenUrlInDefaultBrowser( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _url );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openMail(const Char * _email, const Char * _subject, const Char * _body )
    {
        bool result = Mengine::Helper::AndroidOpenMail( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _email, _subject, _body );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEnv * AndroidEnvironmentService::getJENV() const
    {
        return m_jenv;
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
        static jmethodID jmethodID_getAndroidId = m_jenv->GetMethodID( g_jclass_MengineActivity, "getAndroidId", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getAndroidId != nullptr, "invalid get android method 'getAndroidId'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getAndroidId );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _androidId, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceName(Char * _deviceName, size_t _capacity ) const
    {
        static jmethodID jmethodID_getDeviceName = m_jenv->GetMethodID( g_jclass_MengineActivity, "getDeviceName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getDeviceName != nullptr, "invalid get android method 'getDeviceName'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getDeviceName );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceName, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidPackageName(Char * _packageName, size_t _capacity ) const
    {
        static jmethodID jmethodID_getPackageName = m_jenv->GetMethodID( g_jclass_MengineActivity, "getPackageName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getPackageName != nullptr, "invalid get android method 'getPackageName'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_MengineActivity, jmethodID_getPackageName );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _packageName, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidOpenAssetFile(const Char * _path )
    {
        int32_t fileId = Helper::AndroidOpenAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidAvailableAssetFile(int32_t _fileId )
    {
        int32_t available = Helper::AndroidAvailableAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidReadAssetFile(int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        int32_t read = Helper::AndroidReadAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidEnvironmentService::androidSkipAssetFile(int32_t _fileId, int32_t _offset )
    {
        int32_t skip = Helper::AndroidSkipAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidResetAssetFile(int32_t _fileId )
    {
        Helper::AndroidResetAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::androidCloseAssetFile(int32_t _fileId )
    {
        Helper::AndroidCloseAssetFile( m_jenv, g_jclass_MengineActivity, g_jobject_MengineActivity, _fileId );
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
        static jmethodID jmethodID_onMengineAnalyticsEvent = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineAnalyticsEvent", "(Ljava/lang/String;JLjava/util/Map;)V" );

        MENGINE_ASSERTION( jmethodID_onMengineAnalyticsEvent != nullptr, "invalid get android method 'onAnalyticsEvent'" );

        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        jobject eventName_jobject = this->makeJObjectString( eventName_str );

        TimeMilliseconds eventTimestamp = _event->getTimestamp();

        jobject eventTimestamp_jobject = this->makeJObjectLong( eventTimestamp );

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = this->makeJObjectHashMap( countParameters );

        _event->foreachParameters( [this, parameters_jobject]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
           {
               const Char * name_str = _name.c_str();

               jobject name_jvalue = this->makeJObjectString( name_str );

               EAnalyticsEventParameterType parameterType = _parameter->getType();

               jobject parameter_jobject = nullptr;

               switch( parameterType )
               {
               case EAEPT_BOOLEAN:
                   {
                       AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                       bool parameter_value = parameter_boolean->resolveValue();

                       parameter_jobject = this->makeJObjectBoolean( parameter_value );
                   }break;
               case EAEPT_INTEGER:
                   {
                       AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                       int64_t parameter_value = parameter_integer->resolveValue();

                       parameter_jobject = this->makeJObjectLong( parameter_value );
                   }break;
               case EAEPT_DOUBLE:
                   {
                       AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                       double parameter_value = parameter_double->resolveValue();

                       parameter_jobject = this->makeJObjectDouble( parameter_value );
                   }break;
               case EAEPT_STRING:
                   {
                       AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                       const String & parameter_value = parameter_string->resolveValue();

                       const Char * parameter_value_str = parameter_value.c_str();

                       parameter_jobject = this->makeJObjectString( parameter_value_str );
                   }break;
               }

               static jmethodID Map_put = m_jenv->GetMethodID( m_jclass_Map, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;" );

               m_jenv->CallObjectMethod( parameters_jobject, Map_put, name_jvalue, parameter_jobject );

               m_jenv->DeleteLocalRef( parameter_jobject );
           });

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineAnalyticsEvent, eventName_jobject, eventTimestamp_jobject, parameters_jobject );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationRun_()
    {
        static jmethodID jmethodID_onMengineApplicationRun = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationRun", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationRun != nullptr, "invalid get android method 'onMengineApplicationRun'" );

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationRun );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationReady_()
    {
        static jmethodID jmethodID_onMengineApplicationReady = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationReady", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationReady != nullptr, "invalid get android method 'onMengineApplicationReady'" );

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationReady );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationStop_()
    {
        static jmethodID jmethodID_onMengineApplicationStop = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineApplicationStop", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationStop != nullptr, "invalid get android method 'onMengineApplicationStop'" );

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineApplicationStop );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_()
    {
        static jmethodID jmethodID_onMengineInitializeBaseServices = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineInitializeBaseServices", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineInitializeBaseServices != nullptr, "invalid get android method 'onMengineInitializeBaseServices'" );

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineInitializeBaseServices );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperCreateApplication_()
    {
        static jmethodID jmethodID_onMengineCreateApplication = m_jenv->GetMethodID( g_jclass_MengineActivity, "onMengineCreateApplication", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineCreateApplication != nullptr, "invalid get android method 'onMengineCreateApplication'" );

        m_jenv->CallVoidMethod( g_jobject_MengineActivity, jmethodID_onMengineCreateApplication );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectBoolean(bool _value )
    {
        static jmethodID Boolean_constructor = m_jenv->GetMethodID( m_jclass_Boolean, "<init>", "(Z)V" );

        MENGINE_ASSERTION( Boolean_constructor != nullptr, "invalid get android method 'Boolean <init> (Z)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_Boolean, Boolean_constructor, _value );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectInteger(int32_t _value )
    {
        static jmethodID Integer_constructor = m_jenv->GetMethodID( m_jclass_Integer, "<init>", "(I)V" );

        MENGINE_ASSERTION( Integer_constructor != nullptr, "invalid get android method 'Integer <init> (I)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_Integer, Integer_constructor, _value );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectLong(int64_t _value )
    {
        static jmethodID Long_constructor = m_jenv->GetMethodID( m_jclass_Long, "<init>", "(J)V" );

        MENGINE_ASSERTION( Long_constructor != nullptr, "invalid get android method 'Long <init> (J)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_Long, Long_constructor, _value );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectFloat(float _value )
    {
        static jmethodID Float_constructor = m_jenv->GetMethodID( m_jclass_Float, "<init>", "(F)V" );

        MENGINE_ASSERTION( Float_constructor != nullptr, "invalid get android method 'Float <init> (F)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_Float, Float_constructor, _value );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectDouble(double _value )
    {
        static jmethodID Double_constructor = m_jenv->GetMethodID( m_jclass_Double, "<init>", "(D)V" );

        MENGINE_ASSERTION( Double_constructor != nullptr, "invalid get android method 'Double <init> (D)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_Double, Double_constructor, _value );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectString(const Char * _value )
    {
        static jmethodID String_constructor = m_jenv->GetMethodID( m_jclass_String, "<init>", "(Ljava/lang/String;)V" );

        MENGINE_ASSERTION( String_constructor != nullptr, "invalid get android method 'String <init> (Ljava/lang/String;)V'" );

        jstring value_jstring = m_jenv->NewStringUTF( _value );

        jobject value_jobject = m_jenv->NewObject( m_jclass_String, String_constructor, value_jstring );

        m_jenv->DeleteLocalRef( value_jstring );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::makeJObjectHashMap( int32_t _count )
    {
        static jmethodID HashMap_constructor = m_jenv->GetMethodID( m_jclass_HashMap, "<init>", "(I)V" );

        MENGINE_ASSERTION( HashMap_constructor != nullptr, "invalid get android method 'HashMap <init> (I)V'" );

        jobject value_jobject = m_jenv->NewObject( m_jclass_HashMap, HashMap_constructor, _count );

        return value_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
}
