#include "SDLPlatformAndroidExtension.h"

#include "Interface/PlatformInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Environment/Android/AndroidUtils.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static jclass g_jclass_activity;
static jobject g_jobject_activity;
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1setMengineAndroidActivityJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_activity = (jclass)env->NewGlobalRef( cls );
        g_jobject_activity = (jclass)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1quitMengineAndroidActivityJNI )(JNIEnv * env, jclass cls)
    {
        PLATFORM_SERVICE()
            ->closeWindow();
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getCompanyName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        jstring result = env->NewStringUTF( companyName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getProjectName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        jstring result = env->NewStringUTF( projectName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getProjectVersion )(JNIEnv * env, jclass cls)
    {
        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        jint result = (jint)projectVersion;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isDebugMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDebugMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isDevelopmentMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDevelopmentMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isMasterRelease )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isMasterRelease();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isBuildPublish )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isBuildPublish();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getEngineGITSHA1 )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * ENGINE_GIT_SHA1 = Mengine::Helper::getEngineGITSHA1();

        jstring result = env->NewStringUTF( ENGINE_GIT_SHA1 );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildTimestamp )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_TIMESTAMP = Mengine::Helper::getBuildTimestamp();

        jstring result = env->NewStringUTF( BUILD_TIMESTAMP );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildUsername )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_USERNAME = Mengine::Helper::getBuildUsername();

        jstring result = env->NewStringUTF( BUILD_USERNAME );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildVersion )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();

        jstring result = env->NewStringUTF( BUILD_VERSION );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getConfigValue )(JNIEnv * env, jclass cls, jstring _section, jstring _key, jstring _default)
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
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getConfigValueBoolean )(JNIEnv * env, jclass cls, jstring _section, jstring _key, jboolean _default)
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
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1hasOption )(JNIEnv * env, jclass cls, jstring _option)
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
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidNativeMengine_1logInfo )(JNIEnv * env, jclass cls, jstring _msg)
    {
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
            );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidNativeMengine_1logWarning )(JNIEnv * env, jclass cls, jstring _msg)
    {
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_WARNING, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
            );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidNativeMengine_1logMessage )(JNIEnv * env, jclass cls, jstring _msg)
    {
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
            );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidNativeMengine_1logMessageRelease )(JNIEnv * env, jclass cls, jstring _msg)
    {
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
            );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidNativeMengine_1logError )(JNIEnv * env, jclass cls, jstring _msg)
    {
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL( "android" ), Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::ELF_FLAG_NONE )("%s"
            , msg_str
            );

        env->ReleaseStringUTFChars( _msg, msg_str );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLPlatformAndroidExtension::SDLPlatformAndroidExtension()
        : m_jenv( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatformAndroidExtension::~SDLPlatformAndroidExtension()
    {
        MENGINE_ASSERTION_OBSERVABLE( this, "service 'SDLPlatformAndroidExtension'" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatformAndroidExtension::initializePlatformExtension()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        m_jenv = env;

        m_androidEventationHub = Helper::makeFactorableUnique<AndroidEventationHub>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_androidEventationHub->initialize() == false )
        {
            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_RUN, &SDLPlatformAndroidExtension::notifyApplicationRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_READY, &SDLPlatformAndroidExtension::notifyApplicationReady_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_APPLICATION_STOP, &SDLPlatformAndroidExtension::notifyApplicationStop_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &SDLPlatformAndroidExtension::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS(NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &SDLPlatformAndroidExtension::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::finalizePlatformExtension()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_RUN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_READY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_STOP );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        m_androidEventationHub->finalize();
        m_androidEventationHub = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatformAndroidExtension::openUrlInDefaultBrowser(const Char * _url )
    {
        bool result = Mengine::Helper::AndroidOpenUrlInDefaultBrowser( m_jenv, g_jclass_activity, g_jobject_activity, _url );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatformAndroidExtension::openMail(const Char * _email, const Char * _subject, const Char * _body )
    {
        bool result = Mengine::Helper::AndroidOpenMail( m_jenv, g_jclass_activity, g_jobject_activity, _email, _subject, _body );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEnv * SDLPlatformAndroidExtension::getJENV() const
    {
        return m_jenv;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass SDLPlatformAndroidExtension::getJClassActivity() const
    {
        return g_jclass_activity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject SDLPlatformAndroidExtension::getJObjectActivity() const
    {
        return g_jobject_activity;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatformAndroidExtension::getAndroidId( Char * _androidId, size_t _capacity ) const
    {
        static jmethodID jmethodID_getAndroidId = m_jenv->GetMethodID( g_jclass_activity, "getAndroidId", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getAndroidId != nullptr, "invalid get android method 'getAndroidId'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_activity, jmethodID_getAndroidId );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _androidId, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatformAndroidExtension::getDeviceName( Char * _deviceName, size_t _capacity ) const
    {
        static jmethodID jmethodID_getDeviceName = m_jenv->GetMethodID( g_jclass_activity, "getDeviceName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getDeviceName != nullptr, "invalid get android method 'getDeviceName'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_activity, jmethodID_getDeviceName );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceName, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatformAndroidExtension::getAndroidPackageName( Char * _packageName, size_t _capacity ) const
    {
        static jmethodID jmethodID_getPackageName = m_jenv->GetMethodID( g_jclass_activity, "getPackageName", "()Ljava/lang/String;" );

        MENGINE_ASSERTION( jmethodID_getPackageName != nullptr, "invalid get android method 'getPackageName'" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( g_jobject_activity, jmethodID_getPackageName );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = m_jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _packageName, jStringValue, _capacity );

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        m_androidEventationHub->invoke();

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatformAndroidExtension::androidOpenAssetFile(const Char * _path )
    {
        int32_t fileId = Helper::AndroidOpenAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatformAndroidExtension::androidAvailableAssetFile(int32_t _fileId )
    {
        int32_t available = Helper::AndroidAvailableAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatformAndroidExtension::androidReadAssetFile(int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        int32_t read = Helper::AndroidReadAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatformAndroidExtension::androidSkipAssetFile(int32_t _fileId, int32_t _offset )
    {
        int32_t skip = Helper::AndroidSkipAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::androidResetAssetFile(int32_t _fileId )
    {
        Helper::AndroidResetAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::androidCloseAssetFile(int32_t _fileId )
    {
        Helper::AndroidCloseAssetFile( m_jenv, g_jclass_activity, g_jobject_activity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::addAndroidEventation(const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->addAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::removeAndroidEventation(const AndroidEventationInterfacePtr & _eventation )
    {
        m_androidEventationHub->removeAndroidEventation( _eventation );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::notifyApplicationRun_()
    {
        static jmethodID jmethodID_onMengineApplicationRun = m_jenv->GetMethodID( g_jclass_activity, "onMengineApplicationRun", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationRun != nullptr, "invalid get android method 'onMengineApplicationRun'" );

        m_jenv->CallVoidMethod( g_jobject_activity, jmethodID_onMengineApplicationRun );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::notifyApplicationReady_()
    {
        static jmethodID jmethodID_onMengineApplicationReady = m_jenv->GetMethodID( g_jclass_activity, "onMengineApplicationReady", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationReady != nullptr, "invalid get android method 'onMengineApplicationReady'" );

        m_jenv->CallVoidMethod( g_jobject_activity, jmethodID_onMengineApplicationReady );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::notifyApplicationStop_()
    {
        static jmethodID jmethodID_onMengineApplicationStop = m_jenv->GetMethodID( g_jclass_activity, "onMengineApplicationStop", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineApplicationStop != nullptr, "invalid get android method 'onMengineApplicationStop'" );

        m_jenv->CallVoidMethod( g_jobject_activity, jmethodID_onMengineApplicationStop );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::notifyBootstrapperInitializeBaseServices_()
    {
        static jmethodID jmethodID_onMengineInitializeBaseServices = m_jenv->GetMethodID( g_jclass_activity, "onMengineInitializeBaseServices", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineInitializeBaseServices != nullptr, "invalid get android method 'onMengineInitializeBaseServices'" );

        m_jenv->CallVoidMethod( g_jobject_activity, jmethodID_onMengineInitializeBaseServices );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatformAndroidExtension::notifyBootstrapperCreateApplication_()
    {
        static jmethodID jmethodID_onMengineCreateApplication = m_jenv->GetMethodID( g_jclass_activity, "onMengineCreateApplication", "()V" );

        MENGINE_ASSERTION( jmethodID_onMengineCreateApplication != nullptr, "invalid get android method 'onMengineCreateApplication'" );

        m_jenv->CallVoidMethod( g_jobject_activity, jmethodID_onMengineCreateApplication );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}
