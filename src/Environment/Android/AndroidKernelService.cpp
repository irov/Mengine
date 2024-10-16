#include "AndroidKernelService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidDeclaration.h"
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
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getCompanyName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        jstring result = env->NewStringUTF( companyName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getProjectName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        jstring result = env->NewStringUTF( projectName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1getExtraPreferencesFolderName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char extraPreferencesFolderName[MENGINE_MAX_PATH + 1] = {'\0'};
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
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1deleteCurrentAccount )(JNIEnv * env, jclass cls)
    {
        Mengine::Helper::dispatchMainThreadEvent( []()
        {
            ACCOUNT_SERVICE()
                ->deleteCurrentAccount();
        } );
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

        const Mengine::Char * folderName_str = folderName.c_str();

        jstring result = env->NewStringUTF( folderName_str );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnvironmentService_1writeCurrentLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
    {
        LOGGER_SERVICE()
            ->flushMessages();

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

        if( Mengine::Helper::AndroidWriteMemory( env, memory, _writer ) == false )
        {
            return JNI_FALSE;
        }

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

        if( Mengine::Helper::AndroidWriteMemory( env, memory, _writer ) == false )
        {
            return JNI_FALSE;
        }

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
SERVICE_FACTORY( AndroidKernelService, Mengine::AndroidKernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidKernelService::~AndroidKernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidKernelService::_initializeService()
    {
        m_androidAnalyticsEventProvider = Helper::makeFactorableUnique<AndroidAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( m_androidAnalyticsEventProvider );

        ThreadMutexInterfacePtr mutexCommands = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutexCommands = mutexCommands;

        ThreadMutexInterfacePtr mutexJStrings = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_mutexJStrings = mutexJStrings;

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>(MENGINE_DOCUMENT_FACTORABLE);

        proxyLogger->setVerboseFilter( Mengine::LFILTER_ANDROID );

        if( LOGGER_SERVICE()
            ->registerLogger( proxyLogger ) == true )
        {
            m_proxyLogger = proxyLogger;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_RUN, &AndroidKernelService::notifyPlatformRun_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_PLATFORM_STOP, &AndroidKernelService::notifyPlatformStop_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &AndroidKernelService::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AndroidKernelService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &AndroidKernelService::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_END_UPDATE, &AndroidKernelService::notifyApplicationEndUpdate_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_RUN );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_PLATFORM_STOP );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_END_UPDATE );

        m_mutexJStrings = nullptr;

        if( m_proxyLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_proxyLogger );

            m_proxyLogger = nullptr;
        }

        if( m_androidAnalyticsEventProvider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_androidAnalyticsEventProvider );

            m_androidAnalyticsEventProvider = nullptr;
        }

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
    size_t AndroidKernelService::getAndroidId( Char * const _androidId, size_t _capacity ) const
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getAndroidId", "()Ljava/lang/String;" );

        size_t size = Helper::AndroidCopyStringFromJString( jenv, jReturnValue, _androidId, _capacity );

        jenv->DeleteLocalRef( jReturnValue );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr )
    {
        jsize value_length = _jenv->GetStringLength( _value );

        if( value_length == 0 )
        {
            *_cstr = ConstString::none();

            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutexJStrings );

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
    void AndroidKernelService::addCommand( const LambdaCommand & _command )
    {
        m_mutexCommands->lock();
        m_commandsAux.emplace_back( _command );
        m_mutexCommands->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::invokeCommands()
    {
        m_mutexCommands->lock();
        std::swap( m_commands, m_commandsAux );
        m_commandsAux.clear();
        m_mutexCommands->unlock();

        for( const LambdaCommand & command : m_commands )
        {
            command();
        }

        m_commands.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidKernelService::openUrlInDefaultBrowser( const Char * _url )
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
    bool AndroidKernelService::openMail( const Char * _email, const Char * _subject, const Char * _body )
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
    void AndroidKernelService::notifyPlatformRun_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMenginePlatformRun", "()V" );

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::notifyPlatformStop_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMenginePlatformStop", "()V" );

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::notifyApplicationBeginUpdate_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::notifyApplicationEndUpdate_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::notifyBootstrapperInitializeBaseServices_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMengineInitializeBaseServices", "()V" );

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidKernelService::notifyBootstrapperCreateApplication_()
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidActivityMethod( jenv, "onMengineCreateApplication", "()V" );

        this->invokeCommands();
    }
    //////////////////////////////////////////////////////////////////////////
}