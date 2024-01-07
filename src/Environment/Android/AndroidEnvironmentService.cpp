#include "AndroidEnvironmentService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/SemaphoreServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"

#include "AndroidSemaphoreListener.h"
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
#include "Kernel/StringSlice.h"

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
                Mengine::Char date[256] = {'\0'};
                Mengine::Helper::makeLoggerShortDate( _message.dateTime, "[%02u:%02u:%02u:%04u]", date, 0, 256 );

                jstring jvalue = env->NewStringUTF( date );
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

            Mengine::Char msg[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
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
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1writeOldLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
    {
        const Mengine::MemoryInterfacePtr & memory = LOGGER_SERVICE()
            ->getOldLogMemory();

        if( memory == nullptr )
        {
            return JNI_FALSE;
        }

        jclass jclass_Writer = env->GetObjectClass( _writer );
        jmethodID jmethodID_Writer_write_String = env->GetMethodID( jclass_Writer, "write", "(Ljava/lang/String;)V" );

        const Mengine::Char * data_value = memory->getBuffer();
        size_t data_size = memory->getSize();

        Mengine::Char jvalue_str[1024 + 1] = {'\0'};
        Mengine::Helper::stringSlice( data_value, data_size, jvalue_str, 1024, [env, _writer, jmethodID_Writer_write_String]( const Mengine::Char * _str )
        {
            jstring jvalue = env->NewStringUTF( _str );
            env->CallVoidMethod( _writer, jmethodID_Writer_write_String, jvalue );
            env->DeleteLocalRef( jvalue );
        });

        env->DeleteLocalRef( jclass_Writer );

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
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AnroidEnvironmentService_1activateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::ConstString name = Mengine::Helper::makeConstStringFromJString( env, _name );

        SEMAPHORE_SERVICE()
            ->activateSemaphore( name );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AnroidEnvironmentService_1deactivateSemaphore )(JNIEnv * env, jclass cls, jstring _name)
    {
        Mengine::ConstString name = Mengine::Helper::makeConstStringFromJString( env, _name );

        SEMAPHORE_SERVICE()
                ->deactivateSemaphore( name );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AnroidEnvironmentService_1waitSemaphore )(JNIEnv * env, jclass cls, jstring _name, jobject _cb)
    {
        Mengine::ConstString name = Mengine::Helper::makeConstStringFromJString( env, _name );

        Mengine::AndroidSemaphoreListenerPtr listener = Mengine::Helper::makeFactorableUnique<Mengine::AndroidSemaphoreListener>( MENGINE_DOCUMENT_FUNCTION, env, _cb );

        SEMAPHORE_SERVICE()
            ->waitSemaphore( name, listener );
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
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from(this) );

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        LOGGER_SERVICE()
            ->registerLogger( proxyLogger );

        m_proxyLogger = proxyLogger;

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

        LOGGER_SERVICE()
            ->unregisterLogger( m_proxyLogger );

        m_proxyLogger = nullptr;

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

        jboolean jresult = this->callBooleanActivityMethod( jenv, "linkingOpenURL", "(Ljava/lang/String;)Z", jurl );

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

        jboolean jresult = this->callBooleanActivityMethod( jenv, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jemail, jsubject, jbody );

        jenv->DeleteLocalRef( jemail );
        jenv->DeleteLocalRef( jsubject );
        jenv->DeleteLocalRef( jbody );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::callVoidApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getApplicationMethodID( _jenv, _name, _signature );

        jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        _jenv->CallVoidMethodV( jobjectMengineApplication, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::callVoidActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getActivityMethodID( _jenv, _name, _signature );

        jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        _jenv->CallVoidMethodV( jobjectMengineActivity, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::callObjectActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getActivityMethodID( _jenv, _name, _signature );

        jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        jobject jresult = _jenv->CallObjectMethodV( jobjectMengineActivity, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    jboolean AndroidEnvironmentService::callBooleanActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getActivityMethodID( _jenv, _name, _signature );

        jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        jboolean jresult = _jenv->CallBooleanMethodV( jobjectMengineActivity, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    jint AndroidEnvironmentService::callIntActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getActivityMethodID( _jenv, _name, _signature );

        jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        jint jresult = _jenv->CallIntMethodV( jobjectMengineActivity, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    jlong AndroidEnvironmentService::callLongActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getActivityMethodID( _jenv, _name, _signature );

        jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        jlong jresult = _jenv->CallLongMethodV( jobjectMengineActivity, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidEnvironmentService::getJClass( JNIEnv * _jenv, const Char * _signature ) const
    {
        jclass clazz = _jenv->FindClass( _signature );

        return clazz;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID AndroidEnvironmentService::getApplicationMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const
    {
        jclass jclassMengineApplication = Mengine_JNI_GetJClassMengineApplication();

        jmethodID jmethodID_function = _jenv->GetMethodID( jclassMengineApplication, _name, _signature );

        MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android application method '%s' with signature '%s'"
            , _name
            , _signature
        );

        return jmethodID_function;
    }
    //////////////////////////////////////////////////////////////////////////
    jmethodID AndroidEnvironmentService::getActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature ) const
    {
        jclass jclassMengineActivity = Mengine_JNI_GetJClassMengineActivity();

        jmethodID jmethodID_function = _jenv->GetMethodID( jclassMengineActivity, _name, _signature );

        MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android activity method '%s' with signature '%s'"
        	, _name
        	, _signature
        );

        return jmethodID_function;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidEnvironmentService::callObjectApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... ) const
    {
        jmethodID jmethodId = this->getApplicationMethodID( _jenv, _name, _signature );

        jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _signature );

        jobject result_jobject = _jenv->CallObjectMethodV( jobjectMengineApplication, jmethodId, args );

        MENGINE_VA_LIST_END( args );

        Helper::jEnvExceptionCheck( _jenv );

        return result_jobject;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidEnvironmentService::getAndroidId( Char * _androidId, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)this->callObjectApplicationMethod( jenv, "getAndroidId", "()Ljava/lang/String;" );

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
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)this->callObjectApplicationMethod( jenv, "getDeviceName", "()Ljava/lang/String;" );

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
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring) this->callObjectApplicationMethod(jenv, "getDeviceLanguage", "()Ljava/lang/String;" );

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
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring) this->callObjectApplicationMethod( jenv, "getPackageName", "()Ljava/lang/String;" );

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

        jobject eventName_jobject = Helper::makeJObjectString( jenv, eventName );

        Timestamp eventTimestamp = _event->getTimestamp();

        jobject bases_jobject = Helper::makeJObjectHashMap( jenv, 0 );

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = Helper::makeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
           {
               jobject name_jvalue = Helper::makeJObjectString( jenv, _name );

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

                       parameter_jobject = Helper::makeJObjectString( jenv, parameter_value );
                   }break;
               case EAEPT_CONSTSTRING:
                   {
                       AnalyticsEventParameterConstStringInterfacePtr parameter_conststring = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                       const ConstString & parameter_value = parameter_conststring->resolveValue();

                       parameter_jobject = Helper::makeJObjectString( jenv, parameter_value );
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

        this->callVoidApplicationMethod( jenv, "onMengineAnalyticsEvent", "(Ljava/lang/String;JLjava/util/Map;Ljava/util/Map;)V", eventName_jobject, (jlong)eventTimestamp, bases_jobject, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef(bases_jobject );
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

        jobject screenType_jobject = Helper::makeJObjectString( jenv, _screenType );
        jobject screenName_jobject = Helper::makeJObjectString( jenv, _screenName );

        this->callVoidApplicationMethod( jenv, "onMengineAnalyticsScreenView", "(Ljava/lang/String;Ljava/lang/String;)V", screenType_jobject, screenName_jobject );

        jenv->DeleteLocalRef( screenType_jobject );
        jenv->DeleteLocalRef(screenName_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::onAnalyticsFlush()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        this->callVoidApplicationMethod( jenv, "onMengineAnalyticsFlush", "()V" );
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

        this->callVoidActivityMethod( jenv, "onMenginePlatformRun", "()V" );

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

        this->callVoidActivityMethod( jenv, "onMenginePlatformStop", "()V" );

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

        this->callVoidActivityMethod( jenv, "onMengineInitializeBaseServices", "()V" );

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

        this->callVoidActivityMethod( jenv, "onMengineCreateApplication", "()V" );

        m_androidEventationHub->invoke();
    }
    //////////////////////////////////////////////////////////////////////////
}