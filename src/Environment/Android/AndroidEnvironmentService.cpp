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
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "AndroidSemaphoreListenerInterface.h"
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
extern "C"
{
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

        if( account == nullptr )
        {
            return env->NewStringUTF( "" );
        }

        const Mengine::ConstString & folderName = account->getFolderName();

        jstring result = env->NewStringUTF( folderName.c_str() );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1writeCurrentLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
    {
        const Mengine::ContentInterfacePtr & content = LOGGER_SERVICE()
            ->getCurrentContentLog();

        if( content == nullptr )
        {
            return JNI_FALSE;
        }

        Mengine::MemoryInterfacePtr memory = content->createMemoryFile( false, true, MENGINE_DOCUMENT_FUNCTION );

        if( memory == nullptr )
        {
            return JNI_FALSE;
        }

        Mengine::Helper::AndroidWriteMemory( env, memory, _writer );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1writeOldLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
    {
        const Mengine::MemoryInterfacePtr & memory = LOGGER_SERVICE()
            ->getOldLogMemory();

        if( memory == nullptr )
        {
            return JNI_FALSE;
        }

        Mengine::Helper::AndroidWriteMemory( env, memory, _writer );

        return JNI_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1isDevelopmentMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDevelopmentMode();

        jboolean result = (jboolean)mode;

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
    JNIEXPORT void JNICALL MENGINE_SEMAPHORE_LISTENER_JAVA_INTERFACE( AnroidEnvironmentService_1callMengineSemaphoreListener )(JNIEnv * env, jclass cls, jobject _impl)
    {
        void * impl_MengineFunctorVoid = env->GetDirectBufferAddress( _impl );

        Mengine::AndroidSemaphoreListenerInterface * functor = reinterpret_cast<Mengine::AndroidSemaphoreListenerInterface *>(impl_MengineFunctorVoid);

        functor->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SEMAPHORE_LISTENER_JAVA_INTERFACE( AnroidEnvironmentService_1destroyMengineSemaphoreListener )(JNIEnv * env, jclass cls, jobject _impl)
    {
        void * impl_MengineFunctorVoid = env->GetDirectBufferAddress( _impl );

        Mengine::AndroidSemaphoreListenerInterface * functor = reinterpret_cast<Mengine::AndroidSemaphoreListenerInterface *>(impl_MengineFunctorVoid);

        Mengine::IntrusivePtrBase::intrusive_ptr_dec_ref( functor );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_LOG_JAVA_INTERFACE( AndroidEnvironmentService_1log )(JNIEnv * env, jclass cls, jint _level, jstring _tag, jstring _msg)
    {
        if( SERVICE_IS_INITIALIZE( Mengine::LoggerServiceInterface ) == false )
        {
            return;
        }

        const Mengine::Char * tag_str = env->GetStringUTFChars( _tag, nullptr );
        const Mengine::Char * msg_str = env->GetStringUTFChars( _msg, nullptr );

        Mengine::ELoggerLevel level;
        uint32_t color;

        switch (_level) {
            case Mengine::LM_SILENT:
                return;
            case Mengine::LM_FATAL:
                level = Mengine::LM_FATAL;
                color = Mengine::LCOLOR_RED;
                break;
            case Mengine::LM_MESSAGE_RELEASE:
                level = Mengine::LM_MESSAGE_RELEASE;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE;
                break;
            case Mengine::LM_ERROR:
                level = Mengine::LM_ERROR;
                color = Mengine::LCOLOR_RED;
                break;
            case Mengine::LM_WARNING:
                level = Mengine::LM_WARNING;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN;
                break;
            case Mengine::LM_MESSAGE:
                level = Mengine::LM_MESSAGE;
                color = Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE;
                break;
            case Mengine::LM_INFO:
                level = Mengine::LM_INFO;
                color = Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE;
                break;
            case Mengine::LM_DEBUG:
                level = Mengine::LM_DEBUG;
                color = Mengine::LCOLOR_BLUE;
                break;
            case Mengine::LM_VERBOSE:
                level = Mengine::LM_VERBOSE;
                color = Mengine::LCOLOR_NONE;
                break;
        }

        LOGGER_VERBOSE_LEVEL( "android", level, Mengine::LFILTER_NONE | Mengine::LFILTER_ANDROID, color, nullptr, 0, Mengine::LFLAG_SHORT )("[%s] %s"
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
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        proxyLogger->setVerboseFilter( Mengine::LFILTER_ANDROID );

        if( LOGGER_SERVICE()
            ->registerLogger( proxyLogger ) == true )
        {
            m_proxyLogger = proxyLogger;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_RUN, &AndroidEnvironmentService::notifyPlatformRun_, MENGINE_DOCUMENT_FACTORABLE );
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
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_STOP );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_END_UPDATE );

        if( m_proxyLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_proxyLogger );

            m_proxyLogger = nullptr;
        }

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        m_androidEventationHub->finalize();
        m_androidEventationHub = nullptr;

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        for( IntrusiveListConstStringHolderJString::iterator
             it = m_holdersJString.begin();
             it != m_holdersJString.end(); )
        {
            IntrusiveListConstStringHolderJString::iterator it_erase = it;

            ConstStringHolderJString * holder = *it;
            ++it;

            m_holdersJString.erase( it_erase );

            holder->removeJString( jenv );

            m_poolJString.destroyT( holder );
        }

        m_holdersJString.clear();
        m_poolJString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jurl = jenv->NewStringUTF( _url );

        jboolean jresult = Helper::AndroidCallBooleanActivityMethod( jenv, "linkingOpenURL", "(Ljava/lang/String;)Z", jurl );

        jenv->DeleteLocalRef( jurl );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jemail = jenv->NewStringUTF( _email );
        jstring jsubject = jenv->NewStringUTF( _subject );
        jstring jbody = jenv->NewStringUTF( _body );

        jboolean jresult = Helper::AndroidCallBooleanActivityMethod( jenv, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jemail, jsubject, jbody );

        jenv->DeleteLocalRef( jemail );
        jenv->DeleteLocalRef( jsubject );
        jenv->DeleteLocalRef( jbody );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidId( Char * const _androidId, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getAndroidId", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _androidId, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceName( Char * const _deviceName, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getDeviceName", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceModel( Char * const _deviceModel, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getDeviceModel", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceModel, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getDeviceLanguage", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _deviceLanguage, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getOSVersion( Char * const _osVersion, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getOSVersion", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _osVersion, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getBundleId( Char * const _packageName, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getPackageName", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _packageName, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getSessionId( Char * const _sessionId, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getSessionId", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _sessionId, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getInstallKey( Char * const _installKey, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getInstallKey", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _installKey, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getInstallTimestamp() const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        int64_t jReturnValue = Helper::AndroidCallLongApplicationMethod( jenv, "getInstallTimestamp", "()J" );

        return jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getInstallVersion( Char * const _installVersion, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getInstallVersion", "()Ljava/lang/String;" );

        const Char * jStringValue = jenv->GetStringUTFChars( jReturnValue, nullptr );
        jsize jStringLen = jenv->GetStringLength( jReturnValue );

        MENGINE_STRNCPY( _installVersion, jStringValue, _capacity );

        jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );
        jenv->DeleteLocalRef( jReturnValue );

        return jStringLen;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getInstallRND() const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        int64_t jReturnValue = Helper::AndroidCallLongApplicationMethod( jenv, "getInstallRND", "()J" );

        return jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getSessionIndex() const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        int64_t jReturnValue = Helper::AndroidCallLongApplicationMethod( jenv, "getSessionIndex", "()J" );

        return jReturnValue;
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
            holder->removeJString( _jenv );

            m_poolJString.destroyT( holder );

            return;
        }

        m_holdersJString.push_back( holder );
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
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        const ConstString & eventName = _event->getName();

        jobject eventName_jobject = Helper::AndroidMakeJObjectString( jenv, eventName );

        Timestamp eventTimestamp = _event->getTimestamp();

        jobject bases_jobject = Helper::AndroidMakeJObjectHashMap( jenv, 0 );

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = Helper::AndroidMakeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
           {
               jobject name_jvalue = Helper::AndroidMakeJObjectString(jenv, _name);

               EAnalyticsEventParameterType parameterType = _parameter->getType();

               jobject parameter_jobject = nullptr;

               switch( parameterType )
               {
               case EAEPT_BOOLEAN:
                   {
                       AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                       bool parameter_value = parameter_boolean->resolveValue();

                       parameter_jobject = Helper::AndroidMakeJObjectBoolean( jenv, parameter_value );
                   }break;
               case EAEPT_INTEGER:
                   {
                       AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                       int64_t parameter_value = parameter_integer->resolveValue();

                       parameter_jobject = Helper::AndroidMakeJObjectLong( jenv, parameter_value );
                   }break;
               case EAEPT_DOUBLE:
                   {
                       AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                       double parameter_value = parameter_double->resolveValue();

                       parameter_jobject = Helper::AndroidMakeJObjectDouble( jenv, parameter_value );
                   }break;
               case EAEPT_STRING:
                   {
                       AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                       const String & parameter_value = parameter_string->resolveValue();

                       parameter_jobject = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                   }break;
               case EAEPT_CONSTSTRING:
                   {
                       AnalyticsEventParameterConstStringInterfacePtr parameter_conststring = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                       const ConstString & parameter_value = parameter_conststring->resolveValue();

                       parameter_jobject = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                   }break;
               }

               jobject result_jobject = Helper::AndroidPutJObjectMap( jenv, parameters_jobject, name_jvalue, parameter_jobject );

               jenv->DeleteLocalRef( result_jobject );
               jenv->DeleteLocalRef( parameter_jobject );
           });

        Helper::AndroidCallVoidApplicationMethod( jenv, "onMengineAnalyticsEvent", "(Ljava/lang/String;JLjava/util/Map;Ljava/util/Map;)V", eventName_jobject, (jlong)eventTimestamp, bases_jobject, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef( bases_jobject );
        jenv->DeleteLocalRef( parameters_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject screenType_jobject = Helper::AndroidMakeJObjectString( jenv, _screenType );
        jobject screenName_jobject = Helper::AndroidMakeJObjectString( jenv, _screenName );

        Helper::AndroidCallVoidApplicationMethod( jenv, "onMengineAnalyticsScreenView", "(Ljava/lang/String;Ljava/lang/String;)V", screenType_jobject, screenName_jobject );

        jenv->DeleteLocalRef( screenType_jobject );
        jenv->DeleteLocalRef( screenName_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::onAnalyticsFlush()
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidApplicationMethod( jenv, "onMengineAnalyticsFlush", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyPlatformRun_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMenginePlatformRun", "()V" );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyPlatformStop_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMenginePlatformStop", "()V" );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationBeginUpdate_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyApplicationEndUpdate_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperInitializeBaseServices_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMengineInitializeBaseServices", "()V" );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::notifyBootstrapperCreateApplication_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMengineCreateApplication", "()V" );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}