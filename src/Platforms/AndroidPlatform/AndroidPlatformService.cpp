#include "AndroidPlatformService.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidFragmentHelper.h"
#include "Environment/Android/AndroidSemaphoreListenerInterface.h"
#include "Environment/Android/AndroidKernelServiceInterface.h"

#include "AndroidAnalyticsEventProvider.h"
#include "AndroidProxyLogger.h"
#include "AndroidDynamicLibrary.h"

#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/RandomDevice.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/StatisticHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/Switch.h"
#include "Config/Path.h"

#include <clocale>
#include <ctime>
#include <iomanip>

#include <sys/stat.h>
#include <dlfcn.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE_ENABLE
#define MENGINE_SETLOCALE_ENABLE 1
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE_VALUE
#define MENGINE_SETLOCALE_VALUE "C"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#define MENGINE_DEVELOPMENT_USER_FOLDER_NAME "User"
#endif
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    static volatile bool g_androidPlatformActived = true;
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1getCompanyName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        jstring result = env->NewStringUTF( companyName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1getProjectName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME + 1] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        jstring result = env->NewStringUTF( projectName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1getExtraPreferencesFolderName )(JNIEnv * env, jclass cls)
    {
        Mengine::Path extraPreferencesFolderName = {'\0'};
        PLATFORM_SERVICE()
            ->getExtraPreferencesFolderName( extraPreferencesFolderName );

        jstring result = env->NewStringUTF( extraPreferencesFolderName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1hasCurrentAccount )(JNIEnv * env, jclass cls)
    {
        bool result = ACCOUNT_SERVICE()
            ->hasCurrentAccount();

        return (jboolean)result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1deleteCurrentAccount )(JNIEnv * env, jclass cls)
    {
        Mengine::Helper::dispatchMainThreadEvent( []()
        {
            ACCOUNT_SERVICE()
                ->deleteCurrentAccount();
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1getCurrentAccountFolderName )(JNIEnv * env, jclass cls)
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
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1writeCurrentLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
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
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1writeOldLogToFile )(JNIEnv * env, jclass cls, jobject _writer)
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
    JNIEXPORT jboolean JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1isDevelopmentMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDevelopmentMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL MENGINE_JAVA_INTERFACE( AndroidEnvironmentService_1getProjectVersion )(JNIEnv * env, jclass cls)
    {
        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        jint result = (jint)projectVersion;

        return result;
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1surfaceCreatedEvent )(JNIEnv * env, jclass cls, jobject surface)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceCreatedEvent( nativeWindow );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1surfaceDestroyedEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceDestroyedEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1surfaceChangedEvent )(JNIEnv * env, jclass cls, jobject surface, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceChangedEvent( nativeWindow, surfaceWidth, surfaceHeight, deviceWidth, deviceHeight, rate );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1keyEvent )(JNIEnv * env, jclass cls, jboolean isDown, jint keyCode, jint repeatCount)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeKeyEvent( isDown, keyCode, repeatCount );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1textEvent )(JNIEnv * env, jclass cls, jint unicode)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeTextEvent( unicode );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1touchEvent )(JNIEnv * env, jclass cls, jint action, jint pointerId, jfloat x, jfloat y, jfloat pressure)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeTouchEvent( action, pointerId, x, y, pressure );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1accelerationEvent )(JNIEnv * env, jclass cls, jfloat x, jfloat y, jfloat z)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeAccelerationEvent( x, y, z );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1pauseEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativePauseEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1resumeEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeResumeEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1stopEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeStopEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1startEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeStartEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1restartEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeRestartEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1destroyEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeDestroyEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1clipboardChangedEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeClipboardChangedEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1windowFocusChangedEvent )(JNIEnv * env, jclass cls, jboolean _focus)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeWindowFocusChangedEvent( _focus );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1quitEvent )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        g_androidPlatformActived = false;

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeQuitEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1lowMemory )(JNIEnv * env, jclass cls)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeLowMemoryEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1trimMemory )(JNIEnv * env, jclass cls, jint level)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeTrimMemoryEvent( level );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( AndroidPlatform_1changeLocale )(JNIEnv * env, jclass cls, jstring _language)
    {
        if( g_androidPlatformActived == false )
        {
            return;
        }

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        const Mengine::Char * language_str = env->GetStringUTFChars( _language, nullptr );

        platformExtension->androidNativeChangeLocale( language_str );

        env->ReleaseStringUTFChars( _language, language_str );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT jlong JNICALL MENGINE_JAVA_INTERFACE( AndroidStatistic_1getRenderFrameCount )(JNIEnv * env, jclass cls)
    {
        int64_t Statistic_RenderFrameCount = STATISTIC_GET_INTEGER( Mengine::STATISTIC_RENDER_FRAME_COUNT );

        return Statistic_RenderFrameCount;
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT jlong JNICALL MENGINE_JAVA_INTERFACE( AndroidStatistic_1getAllocatorSize )(JNIEnv * env, jclass cls)
    {
        int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( Mengine::STATISTIC_ALLOCATOR_SIZE );

        return Statistic_AllocatorSize;
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT jlong JNICALL MENGINE_JAVA_INTERFACE( AndroidStatistic_1getRenderTextureAllocSize )(JNIEnv * env, jclass cls)
    {
        int64_t Statistic_RenderTextureAllocSize = STATISTIC_GET_INTEGER( Mengine::STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );

        return Statistic_RenderTextureAllocSize;
    }
    ///////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::AndroidPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidPlatformService::AndroidPlatformService()
        : m_beginTime( 0 )
        , m_activityState( EAS_INITIALIZE )
        , m_nativeWindow( nullptr )
        , m_eglDisplay( EGL_NO_DISPLAY )
        , m_eglSurface( EGL_NO_SURFACE )
        , m_eglContext( EGL_NO_CONTEXT )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_desktop( false )
        , m_touchpad( false )
    {
        StdAlgorithm::fill_n( m_fingers, MENGINE_INPUT_MAX_TOUCH, -1 );

        StdAlgorithm::fill_n( m_currentFingersX, MENGINE_INPUT_MAX_TOUCH, 0.f );
        StdAlgorithm::fill_n( m_currentFingersY, MENGINE_INPUT_MAX_TOUCH, 0.f );
        StdAlgorithm::fill_n( m_currentFingersPressure, MENGINE_INPUT_MAX_TOUCH, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidPlatformService::~AndroidPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t AndroidPlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        _currentPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getUserPath( Char * const _userPath ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( Helper::AndroidGetApplicationFilesDirCanonicalPath( jenv, _userPath ) == false )
        {
            LOGGER_ERROR( "invalid get user path" );

            return 0;
        }

        Path extraPreferencesFolderName = {'\0'};
        size_t ExtraPreferencesFolderNameLen = this->getExtraPreferencesFolderName( extraPreferencesFolderName );

        if( ExtraPreferencesFolderNameLen != 0 )
        {
            Helper::pathAppendA( _userPath, extraPreferencesFolderName, MENGINE_PATH_DELIM_BACKSLASH );
        }

        size_t userPathLen = StdString::strlen( _userPath );

        return userPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        const Char * Project_ExtraPreferencesFolderName = CONFIG_VALUE_STRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( Project_ExtraPreferencesFolderName ) == true, "invalid extra preferences folder name '%s'"
            , Project_ExtraPreferencesFolderName
        );

        StdString::strcpy( _folderName, Project_ExtraPreferencesFolderName );
        
        size_t Project_ExtraPreferencesFolderNameLen = StdString::strlen( Project_ExtraPreferencesFolderName );

        return Project_ExtraPreferencesFolderNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getUserLocaleLanguage( Char * const _userLocale ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jstring jlocale = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getDeviceLanguage", "()Ljava/lang/String;" );

        Char deviceLanguage[128 + 1] = {'\0'};
        size_t deviceLanguageLen = Helper::AndroidCopyStringFromJString( jenv, jlocale, deviceLanguage, 128 );

        jenv->DeleteLocalRef( jlocale );

        if( deviceLanguageLen != MENGINE_LOCALE_LANGUAGE_SIZE )
        {
            LOGGER_ERROR( "invalid get user locale language" );

            return false;
        }

        StdString::strcpy_safe( _userLocale, deviceLanguage, MENGINE_LOCALE_LANGUAGE_SIZE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );

        if( m_nativeWindow == nullptr )
        {
            *_resolution = Resolution( 0.f, 0.f );

            return false;
        }

        int32_t width = ANativeWindow_getWidth( m_nativeWindow );
        int32_t height = ANativeWindow_getHeight( m_nativeWindow );

        *_resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        m_platformTags.clear();

        m_touchpad = true;
        m_desktop = false;

        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

        Char platformTags[1024 + 1] = {'\0'};

        for( const ConstString & tag : m_platformTags.getValues() )
        {
            const Char * tag_str = tag.c_str();

            StdString::strcat( platformTags, tag_str );
            StdString::strcat( platformTags, "-" );
        }

        LOGGER_INFO( "platform", "platform tags: %s"
            , platformTags
        );

        LOGGER_INFO( "platform", "touchpad: %u"
            , m_touchpad
        );

        LOGGER_INFO( "platform", "desktop: %u"
            , m_desktop
        );

        uint32_t deviceSeed = Helper::generateRandomDeviceSeed();

        LOGGER_INFO_PROTECTED( "platform", "Device Seed: %u"
            , deviceSeed
        );

        m_analyticsEventProvider = Helper::makeFactorableUnique<AndroidAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( m_analyticsEventProvider );

        AndroidProxyLoggerPtr proxyLogger = Helper::makeFactorableUnique<AndroidProxyLogger>( MENGINE_DOCUMENT_FACTORABLE );

        if( LOGGER_SERVICE()
            ->registerLogger( proxyLogger ) == true )
        {
            m_proxyLogger = proxyLogger;
        }

        m_nativeWindowMutex = Helper::createThreadMutex(MENGINE_DOCUMENT_FACTORABLE );
        m_eglSurfaceMutex = Helper::createThreadMutex(MENGINE_DOCUMENT_FACTORABLE );
        m_eventsMutex = Helper::createThreadMutex(MENGINE_DOCUMENT_FACTORABLE );

        EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );

        if( eglDisplay == EGL_NO_DISPLAY )
        {
            LOGGER_ERROR( "[egl] invalid get display [%d]"
                , ::eglGetError()
            );

            return false;
        }

        EGLint major;
        EGLint minor;
        if( ::eglInitialize( eglDisplay, &major, &minor ) == EGL_FALSE )
        {
            LOGGER_ERROR( "[egl] invalid initialize [%d]"
                , ::eglGetError()
            );

            return false;
        }

        m_eglDisplay = eglDisplay;

        m_factoryDynamicLibraries = Helper::makeFactoryPool<AndroidDynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &AndroidPlatformService::notifyBootstrapperInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &AndroidPlatformService::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::_runService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "start date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::_stopService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "stop date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::_finalizeService()
    {
        LOGGER_INFO( "platform", "finalize" );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        if( m_proxyLogger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_proxyLogger );

            m_proxyLogger = nullptr;
        }

        if( m_analyticsEventProvider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_analyticsEventProvider );

            m_analyticsEventProvider = nullptr;
        }

        m_active = false;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;

        this->destroyWindow_();

        m_platformTags.clear();

        m_eventsMutex->lock();
        m_eventsAux.clear();
        m_events.clear();
        m_eventsMutex->unlock();

        m_eventsMutex = nullptr;
        m_nativeWindowMutex = nullptr;
        m_eglSurfaceMutex = nullptr;

        m_platformTags.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::runPlatform()
    {
        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f, false, false, false );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentEngine", "platformRun", "()V" );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause )
    {
        MENGINE_UNUSED( _pause );
        MENGINE_UNUSED( _flush );

        bool updating = APPLICATION_SERVICE()
            ->beginUpdate( _frameTime );

        if( updating == true )
        {
            if( m_pauseUpdatingTime >= 0.f )
            {
                _frameTime = m_pauseUpdatingTime;
                m_pauseUpdatingTime = -1.f;
            }

            APPLICATION_SERVICE()
                ->tick( _frameTime );
        }

        APPLICATION_SERVICE()
            ->endUpdate();

        if( m_active == false )
        {
            return;
        }

        if( m_activityState != EAS_RESUME && m_activityState != EAS_START )
        {
            return;
        }

        if( _render == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );
        MENGINE_THREAD_MUTEX_SCOPE( m_eglSurfaceMutex );

        if( m_nativeWindow == nullptr || m_eglSurface == EGL_NO_SURFACE || m_eglContext == EGL_NO_CONTEXT )
        {
            return;
        }

        bool sucessful = APPLICATION_SERVICE()
            ->render();

        if( sucessful == true && _flush == true )
        {
            APPLICATION_SERVICE()
                ->flush();
        }

        if( ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] failed to make current [%d]"
                , ::eglGetError()
            );

            return;
        }

        if( ::eglSwapBuffers( m_eglDisplay, m_eglSurface ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] invalid swap buffers [%d]"
                , ::eglGetError()
            );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::loopPlatform()
    {
        m_prevTime = Helper::getElapsedTime();

        for( ;; )
        {
            double currentTime = Helper::getElapsedTime();

            if( this->updatePlatform() == false )
            {
                break;
            }

            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            this->tickPlatform( frameTime, true, true, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_MESSAGE( "open url in default browser '%s'"
            , _url
        );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jurl = jenv->NewStringUTF( _url );

        jboolean jresult = Helper::AndroidCallBooleanActivityMethod( jenv, "linkingOpenURL", "(Ljava/lang/String;)Z", jurl );

        jenv->DeleteLocalRef( jurl );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        LOGGER_MESSAGE( "open mail '%s' subject '%s' body '%s'"
            , _email
            , _subject
            , _body
        );

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
    bool AndroidPlatformService::openDeleteAccount()
    {
        LOGGER_MESSAGE( "open delete account" );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jboolean jresult = Helper::AndroidCallBooleanActivityMethod( jenv, "linkingOpenDeleteAccount", "()Z" );

        return jresult;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyBootstrapperInitializeBaseServices_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentEngine", "initializeBaseServices", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyBootstrapperCreateApplication_()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentEngine", "createApplication", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentEngine", "platformStop", "()V" );

        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setSleepMode( bool _sleepMode )
    {
        MENGINE_UNUSED( _sleepMode );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getSleepMode() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp AndroidPlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        m_projectTitle.assign( _projectTitle );

        LOGGER_INFO( "platform", "project title: %s"
            , m_projectTitle.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
    {
        LOGGER_INFO( "platform", "create window size %u:%u fullscreen %d"
            , _windowResolution.getWidth()
            , _windowResolution.getHeight()
            , _fullscreen
        );

        if( this->createWindow_( _windowResolution, _fullscreen ) == false )
        {
            return false;
        }

        if( this->applyWindow_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::applyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & AndroidPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
//////////////////////////////////////////////////////////////////////////
    int32_t AndroidPlatformService::pxToWidthDp( int32_t _px ) const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidPlatformService::pxToHeightDp( int32_t _px ) const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr AndroidPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        AndroidDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, "can't create dynamic library '%s'"
            , _dynamicLibraryName
        );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid load]"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );

        if( m_nativeWindow == nullptr )
        {
            *_resolution = Resolution( 0.f, 0.f );

            return false;
        }

        int32_t width = ANativeWindow_getWidth( m_nativeWindow );
        int32_t height = ANativeWindow_getHeight( m_nativeWindow );

        *_resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getAlwaysFullscreen() const
    {
#if defined(MENGINE_PLATFORM_IOS)
        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        return true;
#endif

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isDebuggerPresent() const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jclass debugClass = jenv->FindClass("android/os/Debug");
        jmethodID isDebuggerConnectedMethod = jenv->GetStaticMethodID( debugClass, "isDebuggerConnected", "()Z" );

        jboolean isDebuggerConnected = jenv->CallStaticBooleanMethod( debugClass, isDebuggerConnectedMethod );

        Helper::AndroidEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( debugClass );

        if( isDebuggerConnected == JNI_FALSE )
        {
            return false;
        }

        return true;
#else

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::debugBreak()
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        ::DebugBreak();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::closeWindow()
    {
        //Empty
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::minimizeWindow()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        INPUT_SERVICE()
            ->setCursorPosition( TC_TOUCH0, _pos, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::showKeyboard()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Helper::AndroidCallVoidActivityMethod( jenv,  "showKeyboard", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::hideKeyboard()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Helper::AndroidCallVoidActivityMethod( jenv, "hideKeyboard", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isShowKeyboard() const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        bool result = Helper::AndroidCallBooleanActivityMethod( jenv, "isShowKeyboard", "()Z" );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyCursorModeChanged( bool _mode )
    {
        MENGINE_UNUSED( _mode );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        Char str[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            return;
        }

        jstring j_caption = jenv->NewStringUTF( _caption );
        jstring j_message = jenv->NewStringUTF( str );

        Helper::AndroidCallVoidActivityMethod( jenv, "showAlertDialog", "(Ljava/lang/String;Ljava/lang/String;)V", j_caption, j_message );

        jenv->DeleteLocalRef( j_caption );
        jenv->DeleteLocalRef( j_message );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::setClipboardText( const Char * _value ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jstring j_text = jenv->NewStringUTF( _value );

        Helper::AndroidCallVoidActivityMethod( jenv, "setClipboardText", "(Ljava/lang/String;)V", j_text );

        jenv->DeleteLocalRef( j_text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jstring j_text = (jstring)Helper::AndroidCallObjectActivityMethod( jenv, "getClipboardText", "()Ljava/lang/String;" );

        if( j_text == nullptr )
        {
            return false;
        }

        const Char * text_str = jenv->GetStringUTFChars( j_text, nullptr );

        StdString::strcpy_safe( _value, text_str, _capacity );

        jenv->ReleaseStringUTFChars( j_text, text_str );

        jenv->DeleteLocalRef( j_text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::destroyWindow_()
    {
        LOGGER_INFO( "platform", "destroy window" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        MENGINE_THREAD_MUTEX_SCOPE( m_eglSurfaceMutex );

        if( ::eglTerminate( m_eglDisplay ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] invalid terminate [%d]"
                , ::eglGetError()
            );
        }

        m_eglDisplay = EGL_NO_DISPLAY;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::processEvents_()
    {
        m_eventsMutex->lock();
        std::swap( m_events, m_eventsAux );
        m_eventsAux.clear();
        m_eventsMutex->unlock();

        for( const PlatformUnionEvent & ev : m_events )
        {
            switch( ev.type )
            {
            case PlatformUnionEvent::PET_QUIT:
                {
                    return true;
                }break;
            case PlatformUnionEvent::PET_PAUSE:
                {
                    this->pauseEvent_( ev.data.pause );
                }break;
            case PlatformUnionEvent::PET_RESUME:
                {
                    this->resumeEvent_( ev.data.resume );
                }break;
            case PlatformUnionEvent::PET_STOP:
                {
                    this->stopEvent_( ev.data.stop );
                }break;
            case PlatformUnionEvent::PET_START:
                {
                    this->startEvent_( ev.data.start );
                }break;
            case PlatformUnionEvent::PET_RESTART:
                {
                    this->restartEvent_( ev.data.restart );
                }break;
            case PlatformUnionEvent::PET_DESTROY:
                {
                    this->destroyEvent_( ev.data.destroy );
                }break;
            case PlatformUnionEvent::PET_SURFACE_CREATE:
                {
                    this->surfaceCreateEvent_( ev.data.surfaceCreate );
                }break;
            case PlatformUnionEvent::PET_SURFACE_DESTROY:
                {
                    this->surfaceDestroyEvent_( ev.data.surfaceDestroy );
                }break;
            case PlatformUnionEvent::PET_SURFACE_CHANGED:
                {
                    this->surfaceChangedEvent_( ev.data.surfaceChanged );
                }break;
            case PlatformUnionEvent::PET_CLIPBOARD_CHANGED:
                {
                    this->clipboardChangedEvent_( ev.data.clipboardChanged );
                }break;
            case PlatformUnionEvent::PET_WINDOW_FOCUS_CHANGED:
                {
                    this->windowFocusChangedEvent_( ev.data.windowFocusChanged );
                }break;
            case PlatformUnionEvent::PET_LOW_MEMORY:
                {
                    this->lowMemoryEvent_( ev.data.lowMemory );
                }break;
            case PlatformUnionEvent::PET_TRIM_MEMORY:
                {
                    this->trimMemoryEvent_( ev.data.trimMemory );
                }break;
            case PlatformUnionEvent::PET_CHANGE_LOCALE:
                {
                    this->changeLocaleEvent_( ev.data.changeLocale );
                }break;
            default:
                break;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pushQuitEvent_()
    {
        PlatformUnionEvent ev;

        ev.type = PlatformUnionEvent::PET_QUIT;

        LOGGER_INFO( "platform", "quit event" );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setActive_( float _x, float _y, bool _active )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( TC_TOUCH0, _x, _y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( TC_TOUCH0, _x, _y, 0.f );
        }

        if( nopause == false )
        {
            APPLICATION_SERVICE()
                ->setFocus( m_active );

            INPUT_SERVICE()
                ->onFocus( m_active );

            APPLICATION_SERVICE()
                ->turnSound( m_active );
        }
        else
        {
            APPLICATION_SERVICE()
                ->setFocus( true );

            INPUT_SERVICE()
                ->onFocus( true );

            APPLICATION_SERVICE()
                ->turnSound( true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::initializeFileService()
    {
        FileGroupInterfacePtr defaultFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &defaultFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none(), defaultFileGroup, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FileGroupInterfacePtr globalFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &globalFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), globalFileGroup, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::finalizeFileService()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::androidNativeGetAndroidId( Char * _androidId, size_t _capacity ) const
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring jReturnValue = (jstring)Helper::AndroidCallObjectApplicationMethod( jenv, "getAndroidId", "()Ljava/lang/String;" );

        size_t size = Helper::AndroidCopyStringFromJString( jenv, jReturnValue, _androidId, _capacity );

        jenv->DeleteLocalRef( jReturnValue );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceCreatedEvent( ANativeWindow * _nativeWindow )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );

        MENGINE_ASSERTION_FATAL( m_nativeWindow == nullptr, "native window already created" );

        m_nativeWindow = _nativeWindow;

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_CREATE;
        ev.data.surfaceCreate.nativeWindow = _nativeWindow;

        LOGGER_INFO( "platform", "push surface create event" );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceDestroyedEvent()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );

        MENGINE_ASSERTION_FATAL( m_nativeWindow != nullptr, "native window already destroyed" );

        if( m_nativeWindow != nullptr )
        {
            ANativeWindow_release( m_nativeWindow );
        }

        m_nativeWindow = nullptr;

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_DESTROY;

        LOGGER_INFO( "platform", "push surface destroy event" );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceChangedEvent( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );

        if( m_nativeWindow != nullptr )
        {
            ANativeWindow_release( m_nativeWindow );
        }

        m_nativeWindow = _nativeWindow;

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_CHANGED;
        ev.data.surfaceChanged.nativeWindow = _nativeWindow;
        ev.data.surfaceChanged.surfaceWidth = surfaceWidth;
        ev.data.surfaceChanged.surfaceHeight = surfaceHeight;
        ev.data.surfaceChanged.deviceWidth = deviceWidth;
        ev.data.surfaceChanged.deviceHeight = deviceHeight;

        LOGGER_INFO( "platform", "push surface changed event surface: %d;%d device: %d;%d rate: %f"
            , surfaceWidth
            , surfaceHeight
            , deviceWidth
            , deviceHeight
            , rate
        );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::acquireFingerIndex_( jint _pointerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint fingerId = m_fingers[index];

            if( fingerId == -1 )
            {
                m_fingers[index] = _pointerId;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::releaseFingerIndex_( jint _pointerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint pointerId = m_fingers[index];

            if( pointerId == _pointerId )
            {
                m_fingers[index] = -1;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::getFingerIndex_( jint _pointerId ) const
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint pointerId = m_fingers[index];

            if( pointerId == _pointerId )
            {
                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure )
    {
        switch( _action )
        {
        case 0: //ACTION_DOWN
        case 5: //ACTION_POINTER_DOWN
            {
                ETouchCode fingerIndex = this->acquireFingerIndex_( _pointerId );

                if( fingerIndex == TC_TOUCH_INVALID )
                {
                    LOGGER_INFO( "platform", "failed to acquire finger index for pointerId %d"
                        , _pointerId
                    );

                    return;
                }

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] down position: %.4f;%.4f pressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                );

                Helper::pushMouseButtonEvent( fingerIndex, _x, _y, MC_LBUTTON, _pressure, true );
            }break;
        case 1: //ACTION_UP
        case 6: //ACTION_POINTER_UP
            {
                ETouchCode fingerIndex = this->releaseFingerIndex_( _pointerId );

                if( fingerIndex == TC_TOUCH_INVALID )
                {
                    LOGGER_INFO( "platform", "failed to release finger index for pointerId %d"
                        , _pointerId
                    );

                    return;
                }

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] up position: %.4f;%.4f pressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                );

                Helper::pushMouseButtonEvent( fingerIndex, _x, _y, MC_LBUTTON, _pressure, false );
            }break;
        case 2: //ACTION_MOVE
            {
                ETouchCode fingerIndex = this->getFingerIndex_( _pointerId );

                if( fingerIndex == TC_TOUCH_INVALID )
                {
                    LOGGER_INFO( "platform", "failed to get finger index for pointerId %d"
                        , _pointerId
                    );

                    return;
                }

                jfloat previousX = m_currentFingersX[fingerIndex];
                jfloat previousY = m_currentFingersY[fingerIndex];
                jfloat previousPressure = m_currentFingersPressure[fingerIndex];

                jfloat dx = _x - previousX;
                jfloat dy = _y - previousY;
                jfloat dpressure = _pressure - previousPressure;

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] move position: %.4f;%.4f pressure: %.4f delta: %.4f;%.4f dpressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                    , dx
                    , dy
                    , dpressure
                );

                Helper::pushMouseMoveEvent( fingerIndex, _x, _y, dx, dy, _pressure, dpressure );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeAccelerationEvent( jfloat _dx, jfloat _dy, jfloat _dz )
    {
        Helper::pushAccelerometerEvent( _dx, _dy, _dz );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount )
    {
        static EKeyCode Android_Keycodes[] = {
            KC_UNASSIGNED,          /* AKEYCODE_UNKNOWN */
            KC_UNASSIGNED,         /* AKEYCODE_SOFT_LEFT */
            KC_UNASSIGNED,        /* AKEYCODE_SOFT_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_HOME */
            KC_BROWSER_BACK,          /* AKEYCODE_BACK */
            KC_UNASSIGNED,             /* AKEYCODE_CALL */
            KC_UNASSIGNED,          /* AKEYCODE_ENDCALL */
            KC_0,                /* AKEYCODE_0 */
            KC_1,                /* AKEYCODE_1 */
            KC_2,                /* AKEYCODE_2 */
            KC_3,                /* AKEYCODE_3 */
            KC_4,                /* AKEYCODE_4 */
            KC_5,                /* AKEYCODE_5 */
            KC_6,                /* AKEYCODE_6 */
            KC_7,                /* AKEYCODE_7 */
            KC_8,                /* AKEYCODE_8 */
            KC_9,                /* AKEYCODE_9 */
            KC_UNASSIGNED,          /* AKEYCODE_STAR */
            KC_UNASSIGNED,          /* AKEYCODE_POUND */
            KC_UP,               /* AKEYCODE_DPAD_UP */
            KC_DOWN,             /* AKEYCODE_DPAD_DOWN */
            KC_LEFT,             /* AKEYCODE_DPAD_LEFT */
            KC_RIGHT,            /* AKEYCODE_DPAD_RIGHT */
            KC_UNASSIGNED,           /* AKEYCODE_DPAD_CENTER */
            KC_UNASSIGNED,         /* AKEYCODE_VOLUME_UP */
            KC_UNASSIGNED,       /* AKEYCODE_VOLUME_DOWN */
            KC_UNASSIGNED,            /* AKEYCODE_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_CAMERA */
            KC_UNASSIGNED,            /* AKEYCODE_CLEAR */
            KC_A,                /* AKEYCODE_A */
            KC_B,                /* AKEYCODE_B */
            KC_C,                /* AKEYCODE_C */
            KC_D,                /* AKEYCODE_D */
            KC_E,                /* AKEYCODE_E */
            KC_F,                /* AKEYCODE_F */
            KC_G,                /* AKEYCODE_G */
            KC_H,                /* AKEYCODE_H */
            KC_I,                /* AKEYCODE_I */
            KC_J,                /* AKEYCODE_J */
            KC_K,                /* AKEYCODE_K */
            KC_L,                /* AKEYCODE_L */
            KC_M,                /* AKEYCODE_M */
            KC_N,                /* AKEYCODE_N */
            KC_O,                /* AKEYCODE_O */
            KC_P,                /* AKEYCODE_P */
            KC_Q,                /* AKEYCODE_Q */
            KC_R,                /* AKEYCODE_R */
            KC_S,                /* AKEYCODE_S */
            KC_T,                /* AKEYCODE_T */
            KC_U,                /* AKEYCODE_U */
            KC_V,                /* AKEYCODE_V */
            KC_W,                /* AKEYCODE_W */
            KC_X,                /* AKEYCODE_X */
            KC_Y,                /* AKEYCODE_Y */
            KC_Z,                /* AKEYCODE_Z */
            KC_UNASSIGNED,            /* AKEYCODE_COMMA */
            KC_UNASSIGNED,           /* AKEYCODE_PERIOD */
            KC_LMENU,             /* AKEYCODE_ALT_LEFT */
            KC_RMENU,             /* AKEYCODE_ALT_RIGHT */
            KC_LSHIFT,           /* AKEYCODE_SHIFT_LEFT */
            KC_RSHIFT,           /* AKEYCODE_SHIFT_RIGHT */
            KC_TAB,              /* AKEYCODE_TAB */
            KC_SPACE,            /* AKEYCODE_SPACE */
            KC_UNASSIGNED,          /* AKEYCODE_SYM */
            KC_UNASSIGNED,              /* AKEYCODE_EXPLORER */
            KC_UNASSIGNED,             /* AKEYCODE_ENVELOPE */
            KC_RETURN,           /* AKEYCODE_ENTER */
            KC_BACK,        /* AKEYCODE_DEL */
            KC_UNASSIGNED,            /* AKEYCODE_GRAVE */
            KC_OEM_MINUS,            /* AKEYCODE_MINUS */
            KC_OEM_PLUS,           /* AKEYCODE_EQUALS */
            KC_OEM_4,      /* AKEYCODE_LEFT_BRACKET */
            KC_OEM_6,     /* AKEYCODE_RIGHT_BRACKET */
            KC_UNASSIGNED,        /* AKEYCODE_BACKSLASH */
            KC_UNASSIGNED,        /* AKEYCODE_SEMICOLON */
            KC_UNASSIGNED,       /* AKEYCODE_APOSTROPHE */
            KC_UNASSIGNED,            /* AKEYCODE_SLASH */
            KC_UNASSIGNED,          /* AKEYCODE_AT */
            KC_UNASSIGNED,          /* AKEYCODE_NUM */
            KC_UNASSIGNED,          /* AKEYCODE_HEADSETHOOK */
            KC_UNASSIGNED,          /* AKEYCODE_FOCUS */
            KC_UNASSIGNED,          /* AKEYCODE_PLUS */
            KC_MENU,             /* AKEYCODE_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_NOTIFICATION */
            KC_UNASSIGNED,        /* AKEYCODE_SEARCH */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_PLAY_PAUSE */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_STOP */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_NEXT */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_PREVIOUS */
            KC_UNASSIGNED,      /* AKEYCODE_MEDIA_REWIND */
            KC_UNASSIGNED, /* AKEYCODE_MEDIA_FAST_FORWARD */
            KC_UNASSIGNED,             /* AKEYCODE_MUTE */
            KC_PRIOR,           /* AKEYCODE_PAGE_UP */
            KC_NEXT,         /* AKEYCODE_PAGE_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_PICTSYMBOLS */
            KC_UNASSIGNED,          /* AKEYCODE_SWITCH_CHARSET */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_A */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_B */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_C */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_X */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_Y */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_Z */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_L1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_R1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_L2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_R2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_THUMBL */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_THUMBR */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_START */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_SELECT */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_MODE */
            KC_ESCAPE,           /* AKEYCODE_ESCAPE */
            KC_DELETE,           /* AKEYCODE_FORWARD_DEL */
            KC_LCONTROL,            /* AKEYCODE_CTRL_LEFT */
            KC_RCONTROL,            /* AKEYCODE_CTRL_RIGHT */
            KC_CAPITAL,         /* AKEYCODE_CAPS_LOCK */
            KC_UNASSIGNED,       /* AKEYCODE_SCROLL_LOCK */
            KC_UNASSIGNED,             /* AKEYCODE_META_LEFT */
            KC_UNASSIGNED,             /* AKEYCODE_META_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_FUNCTION */
            KC_UNASSIGNED,      /* AKEYCODE_SYSRQ */
            KC_PAUSE,            /* AKEYCODE_BREAK */
            KC_HOME,             /* AKEYCODE_MOVE_HOME */
            KC_END,              /* AKEYCODE_MOVE_END */
            KC_INSERT,           /* AKEYCODE_INSERT */
            KC_UNASSIGNED,       /* AKEYCODE_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_PLAY */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_PAUSE */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_CLOSE */
            KC_UNASSIGNED,            /* AKEYCODE_MEDIA_EJECT */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_RECORD */
            KC_F1,               /* AKEYCODE_F1 */
            KC_F2,               /* AKEYCODE_F2 */
            KC_F3,               /* AKEYCODE_F3 */
            KC_F4,               /* AKEYCODE_F4 */
            KC_F5,               /* AKEYCODE_F5 */
            KC_F6,               /* AKEYCODE_F6 */
            KC_F7,               /* AKEYCODE_F7 */
            KC_F8,               /* AKEYCODE_F8 */
            KC_F9,               /* AKEYCODE_F9 */
            KC_F10,              /* AKEYCODE_F10 */
            KC_F11,              /* AKEYCODE_F11 */
            KC_F12,              /* AKEYCODE_F12 */
            KC_UNASSIGNED,          /* AKEYCODE_NUM_LOCK */
            KC_NUMPAD0,             /* AKEYCODE_NUMPAD_0 */
            KC_NUMPAD1,             /* AKEYCODE_NUMPAD_1 */
            KC_NUMPAD2,             /* AKEYCODE_NUMPAD_2 */
            KC_NUMPAD3,             /* AKEYCODE_NUMPAD_3 */
            KC_NUMPAD4,             /* AKEYCODE_NUMPAD_4 */
            KC_NUMPAD5,             /* AKEYCODE_NUMPAD_5 */
            KC_NUMPAD6,             /* AKEYCODE_NUMPAD_6 */
            KC_NUMPAD7,             /* AKEYCODE_NUMPAD_7 */
            KC_NUMPAD8,             /* AKEYCODE_NUMPAD_8 */
            KC_NUMPAD9,             /* AKEYCODE_NUMPAD_9 */
            KC_DIVIDE,        /* AKEYCODE_NUMPAD_DIVIDE */
            KC_UNASSIGNED,      /* AKEYCODE_NUMPAD_MULTIPLY */
            KC_SUBTRACT,         /* AKEYCODE_NUMPAD_SUBTRACT */
            KC_ADD,          /* AKEYCODE_NUMPAD_ADD */
            KC_UNASSIGNED,        /* AKEYCODE_NUMPAD_DOT */
            KC_UNASSIGNED,         /* AKEYCODE_NUMPAD_COMMA */
            KC_UNASSIGNED,         /* AKEYCODE_NUMPAD_ENTER */
            KC_UNASSIGNED,        /* AKEYCODE_NUMPAD_EQUALS */
            KC_UNASSIGNED,     /* AKEYCODE_NUMPAD_LEFT_PAREN */
            KC_UNASSIGNED,    /* AKEYCODE_NUMPAD_RIGHT_PAREN */
            KC_UNASSIGNED,          /* AKEYCODE_VOLUME_MUTE */
            KC_UNASSIGNED,          /* AKEYCODE_INFO */
            KC_UNASSIGNED,          /* AKEYCODE_CHANNEL_UP */
            KC_UNASSIGNED,          /* AKEYCODE_CHANNEL_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_ZOOM_IN */
            KC_UNASSIGNED,          /* AKEYCODE_ZOOM_OUT */
            KC_UNASSIGNED,          /* AKEYCODE_TV */
            KC_UNASSIGNED,          /* AKEYCODE_WINDOW */
            KC_UNASSIGNED,          /* AKEYCODE_GUIDE */
            KC_UNASSIGNED,          /* AKEYCODE_DVR */
            KC_UNASSIGNED,     /* AKEYCODE_BOOKMARK */
            KC_UNASSIGNED,          /* AKEYCODE_CAPTIONS */
            KC_UNASSIGNED,          /* AKEYCODE_SETTINGS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_STB_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_STB_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_AVR_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_AVR_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_RED */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_GREEN */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_YELLOW */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_BLUE */
            KC_UNASSIGNED,          /* AKEYCODE_APP_SWITCH */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_3 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_4 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_5 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_6 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_7 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_8 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_9 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_10 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_11 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_12 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_13 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_14 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_15 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_16 */
            KC_UNASSIGNED,          /* AKEYCODE_LANGUAGE_SWITCH */
            KC_UNASSIGNED,          /* AKEYCODE_MANNER_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_3D_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_CONTACTS */
            KC_UNASSIGNED,          /* AKEYCODE_CALENDAR */
            KC_UNASSIGNED,          /* AKEYCODE_MUSIC */
            KC_UNASSIGNED,       /* AKEYCODE_CALCULATOR */
            KC_UNASSIGNED,            /* AKEYCODE_ZENKAKU_HANKAKU */
            KC_UNASSIGNED,          /* AKEYCODE_EISU */
            KC_UNASSIGNED,   /* AKEYCODE_MUHENKAN */
            KC_UNASSIGNED,   /* AKEYCODE_HENKAN */
            KC_UNASSIGNED,            /* AKEYCODE_KATAKANA_HIRAGANA */
            KC_UNASSIGNED,   /* AKEYCODE_YEN */
            KC_UNASSIGNED,          /* AKEYCODE_RO */
            KC_UNASSIGNED,          /* AKEYCODE_KANA */
            KC_UNASSIGNED,          /* AKEYCODE_ASSIST */
            KC_UNASSIGNED,   /* AKEYCODE_BRIGHTNESS_DOWN */
            KC_UNASSIGNED,     /* AKEYCODE_BRIGHTNESS_UP */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_AUDIO_TRACK */
            KC_UNASSIGNED,            /* AKEYCODE_SLEEP */
            KC_UNASSIGNED,          /* AKEYCODE_WAKEUP */
            KC_UNASSIGNED,          /* AKEYCODE_PAIRING */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_TOP_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_11 */
            KC_UNASSIGNED,          /* AKEYCODE_12 */
            KC_UNASSIGNED,          /* AKEYCODE_LAST_CHANNEL */
            KC_UNASSIGNED,          /* AKEYCODE_TV_DATA_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_VOICE_ASSIST */
            KC_UNASSIGNED,          /* AKEYCODE_TV_RADIO_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TELETEXT */
            KC_UNASSIGNED,          /* AKEYCODE_TV_NUMBER_ENTRY */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TERRESTRIAL_ANALOG */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TERRESTRIAL_DIGITAL */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_BS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_CS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_NETWORK */
            KC_UNASSIGNED,          /* AKEYCODE_TV_ANTENNA_CABLE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_3 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_4 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPOSITE_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPOSITE_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPONENT_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPONENT_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_VGA_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_UP */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_TV_ZOOM_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_CONTENTS_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_TV_MEDIA_CONTEXT_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TIMER_PROGRAMMING */
            KC_UNASSIGNED,             /* AKEYCODE_HELP */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_PREVIOUS */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_NEXT */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_IN */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_OUT */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_PRIMARY */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_1 */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_2 */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_3 */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_UP_LEFT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_DOWN_LEFT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_UP_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_DOWN_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_SKIP_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_SKIP_BACKWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_STEP_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_STEP_BACKWARD */
            KC_UNASSIGNED,          /* AKEYCODE_SOFT_SLEEP */
            KC_UNASSIGNED,              /* AKEYCODE_CUT */
            KC_UNASSIGNED,             /* AKEYCODE_COPY */
            KC_UNASSIGNED,            /* AKEYCODE_PASTE */
        };

        if( _keyCode < 0 || _keyCode >= MENGINE_ARRAY_SIZE(Android_Keycodes) )
        {
            return;
        }

        EKeyCode keyCode = Android_Keycodes[_keyCode];

        if( keyCode == KC_UNASSIGNED )
        {
            return;
        }

        jfloat x = m_currentFingersX[0];
        jfloat y = m_currentFingersY[0];
        jfloat pressure = m_currentFingersPressure[0];

        LOGGER_INFO( "platform", "key event: %d down: %d repeat: %d"
            , keyCode
            , _isDown
            , _repeatCount
        );

        Helper::pushKeyEvent( x, y, pressure, keyCode, _isDown, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeTextEvent( jint _unicode )
    {
        jfloat x = m_currentFingersX[0];
        jfloat y = m_currentFingersY[0];
        jfloat pressure = m_currentFingersPressure[0];

        WChar text[2] = {(WChar)_unicode, L'\0'};

        LOGGER_INFO( "platform", "text event: %ls"
            , text
        );

        Helper::pushTextEvent( x, y, pressure, text );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativePauseEvent()
    {
        m_activityState = EAS_PAUSE;

        float x = m_currentFingersX[0];
        float y = m_currentFingersY[0];

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_PAUSE;

        event.data.pause.x = x;
        event.data.pause.y = y;

        LOGGER_INFO( "platform", "pause event: %.4f;%.4f"
            , x
            , y
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeResumeEvent()
    {
        m_activityState = EAS_RESUME;

        float x = m_currentFingersX[0];
        float y = m_currentFingersY[0];

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_RESUME;

        event.data.resume.x = x;
        event.data.resume.y = y;

        LOGGER_INFO( "platform", "resume event: %.4f;%.4f"
            , x
            , y
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeStopEvent()
    {
        m_activityState = EAS_STOP;

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_STOP;

        LOGGER_INFO( "platform", "stop event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeStartEvent()
    {
        m_activityState = EAS_START;

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_START;

        LOGGER_INFO( "platform", "start event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeRestartEvent()
    {
        m_activityState = EAS_RESTART;

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_RESTART;

        LOGGER_INFO( "platform", "restart event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeDestroyEvent()
    {
        m_activityState = EAS_DESTROY;

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_DESTROY;

        LOGGER_INFO( "platform", "destroy event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeClipboardChangedEvent()
    {
        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_CLIPBOARD_CHANGED;

        LOGGER_INFO( "platform", "clipboard changed event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeWindowFocusChangedEvent( jboolean _focus )
    {
        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_WINDOW_FOCUS_CHANGED;

        event.data.windowFocusChanged.focus = _focus;

        LOGGER_INFO( "platform", "window focus changed event: %d"
            , _focus
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeQuitEvent()
    {
        LOGGER_INFO( "platform", "quit event" );

        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeLowMemoryEvent()
    {
        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_LOW_MEMORY;

        LOGGER_INFO( "platform", "low memory event" );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeTrimMemoryEvent(jint _level)
    {
        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_TRIM_MEMORY;

        event.data.trimMemory.level = _level;

        LOGGER_INFO( "platform", "trim memory event: %d"
            , _level
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeChangeLocale( const Mengine::Char * _language )
    {
        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_CHANGE_LOCALE;

        StdString::strcpy_safe( event.data.changeLocale.language, _language, MENGINE_LOCALE_LANGUAGE_SIZE );

        LOGGER_INFO( "platform", "change locale event: %s"
             , _language
         );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pauseEvent_( const PlatformUnionEvent::PlatformPauseEvent & _event )
    {
        float x = _event.x;
        float y = _event.y;

        this->setActive_( x, y, false );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::resumeEvent_( const PlatformUnionEvent::PlatformResumeEvent & _event )
    {
        float x = _event.x;
        float y = _event.y;

        this->setActive_( x, y, true );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::stopEvent_( const PlatformUnionEvent::PlatformStopEvent & _event )
    {
        MENGINE_UNUSED( _event );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::startEvent_( const PlatformUnionEvent::PlatformStartEvent & _event )
    {
        MENGINE_UNUSED( _event );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::restartEvent_( const PlatformUnionEvent::PlatformRestartEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //ToDo
    }
    void AndroidPlatformService::destroyEvent_( const PlatformUnionEvent::PlatformDestroyEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceCreateEvent_( const PlatformUnionEvent::PlatformSurfaceCreateEvent & _event )
    {
        MENGINE_ASSERTION_FATAL( m_eglDisplay != nullptr, "display not created" );
        MENGINE_ASSERTION_FATAL( m_eglSurface == nullptr, "already created egl surface" );
        MENGINE_ASSERTION_FATAL( m_eglContext == nullptr, "already created egl context" );

        LOGGER_INFO( "platform", "surface create event" );

        MENGINE_THREAD_MUTEX_SCOPE( m_nativeWindowMutex );
        MENGINE_THREAD_MUTEX_SCOPE( m_eglSurfaceMutex );

        if( m_nativeWindow == nullptr )
        {
            LOGGER_ERROR( "[egl] native window is null" );

            return;
        }

        int32_t format_got = ANativeWindow_getFormat( m_nativeWindow );

        LOGGER_INFO( "platform", "native window format: %d"
            , format_got
        );

        if( ::eglBindAPI( EGL_OPENGL_ES_API ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] failed to bind ES api [%d]"
                , ::eglGetError()
            );
        }

        EGLint base_red_size = 8;
        EGLint base_green_size = 8;
        EGLint base_blue_size = 8;
        EGLint base_alpha_size = 8;
        EGLint base_depth_size = 24;

        const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, base_red_size,
            EGL_GREEN_SIZE, base_green_size,
            EGL_BLUE_SIZE, base_blue_size,
            EGL_ALPHA_SIZE, base_alpha_size,
            EGL_DEPTH_SIZE, base_depth_size,
            EGL_NONE
        };

        EGLConfig configs[128];
        EGLint numConfigs;
        if( ::eglChooseConfig( m_eglDisplay, configAttribs, configs, MENGINE_ARRAY_SIZE(configs), &numConfigs ) == EGL_FALSE )
        {
            LOGGER_ERROR( "[egl] failed to choose config [%d]"
                , ::eglGetError()
            );

            return;
        }

        if( numConfigs == 0 )
        {
            LOGGER_ERROR( "[egl] no configs found" );

            return;
        }

        EGLint best_config_index = -1;
        EGLint best_config_score = -1;

        for( EGLint index = 0; index != numConfigs; ++index )
        {
            EGLint red;
            EGLint green;
            EGLint blue;
            EGLint alpha;
            EGLint depth;
            ::eglGetConfigAttrib( m_eglDisplay, configs[index], EGL_RED_SIZE, &red );
            ::eglGetConfigAttrib( m_eglDisplay, configs[index], EGL_GREEN_SIZE, &green );
            ::eglGetConfigAttrib( m_eglDisplay, configs[index], EGL_BLUE_SIZE, &blue );
            ::eglGetConfigAttrib( m_eglDisplay, configs[index], EGL_ALPHA_SIZE, &alpha );
            ::eglGetConfigAttrib( m_eglDisplay, configs[index], EGL_DEPTH_SIZE, &depth );

            EGLint red_score = red - base_red_size;
            EGLint green_score = green - base_green_size;
            EGLint blue_score = blue - base_blue_size;
            EGLint alpha_score = alpha - base_alpha_size;
            EGLint depth_score = depth - base_depth_size;

            EGLint total_score = red_score + green_score + blue_score + alpha_score + depth_score;

            if( best_config_score == -1 )
            {
                best_config_index = index;
                best_config_score = total_score;
            }
            else if( total_score < best_config_score )
            {
                best_config_index = index;
                best_config_score = total_score;
            }
        }

        if( best_config_index == -1 )
        {
            LOGGER_ERROR( "[egl] no suitable config found" );

            return;
        }

        EGLConfig best_config = configs[best_config_index];

        EGLint result_red;
        EGLint result_green;
        EGLint result_blue;
        EGLint result_alpha;
        EGLint result_depth;
        ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_RED_SIZE, &result_red );
        ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_GREEN_SIZE, &result_green );
        ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_BLUE_SIZE, &result_blue );
        ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_ALPHA_SIZE, &result_alpha );
        ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_DEPTH_SIZE, &result_depth );

        LOGGER_MESSAGE( "[egl] config red: %d green: %d blue: %d alpha: %d depth: %d"
            , result_red
            , result_green
            , result_blue
            , result_alpha
            , result_depth
        );

        EGLint format_wanted = 0;
        if( ::eglGetConfigAttrib( m_eglDisplay, best_config, EGL_NATIVE_VISUAL_ID, &format_wanted ) == EGL_FALSE )
        {
            LOGGER_ERROR( "[egl] failed to get config attrib [%d]"
                , ::eglGetError()
            );

            return;
        }

        int errSetBuffersGeometry = ANativeWindow_setBuffersGeometry( m_nativeWindow, 0, 0, format_wanted );

        if( errSetBuffersGeometry != 0 )
        {
            LOGGER_ERROR("[egl] failed to set buffers geometry: %d"
                , errSetBuffersGeometry
            );

            return;
        }

        EGLSurface eglSurface = ::eglCreateWindowSurface( m_eglDisplay, best_config, m_nativeWindow, nullptr );

        if( eglSurface == EGL_NO_SURFACE )
        {
            LOGGER_ERROR( "[egl] failed to create window surface [%d]"
                , ::eglGetError()
            );

            return;
        }

        m_eglSurface = eglSurface;

        EGLint width;
        EGLint height;
        ::eglQuerySurface( m_eglDisplay, m_eglSurface, EGL_WIDTH, &width );
        ::eglQuerySurface( m_eglDisplay, m_eglSurface, EGL_HEIGHT, &height );

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        EGLContext eglShareContext = EGL_NO_CONTEXT;

        EGLContext context = ::eglCreateContext( m_eglDisplay, best_config, eglShareContext, contextAttribs );

        if( context == EGL_NO_CONTEXT )
        {
            LOGGER_ERROR( "[egl] failed to create context [%d]"
                , ::eglGetError()
            );

            return;
        }

        m_eglContext = context;

        if( ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext ) == EGL_FALSE )
        {
            LOGGER_ERROR( "[egl] failed to make current [%d]"
                , ::eglGetError()
            );

            return;
        }

        if( ::eglSwapInterval( m_eglDisplay, 1 ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] invalid set swap interval [%d]"
                , ::eglGetError()
            );
        }

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceDestroyEvent_( const PlatformUnionEvent::PlatformSurfaceDestroyEvent & _event )
    {
        LOGGER_INFO( "platform", "surface destroy event" );

        MENGINE_THREAD_MUTEX_SCOPE( m_eglSurfaceMutex );

        if( m_eglDisplay == EGL_NO_DISPLAY )
        {
            return;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        if( ::eglBindAPI( EGL_OPENGL_ES_API ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] failed to bind ES api [%d]"
                , ::eglGetError()
            );
        }

        if( ::eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT ) == EGL_FALSE )
        {
            LOGGER_WARNING( "[egl] failed to clear currents [%d]"
                , ::eglGetError()
            );
        }

        if( m_eglContext != EGL_NO_CONTEXT )
        {
            if( ::eglDestroyContext( m_eglDisplay, m_eglContext ) == EGL_FALSE )
            {
                LOGGER_WARNING( "[egl] failed to destroy context [%d]"
                    , ::eglGetError()
                );
            }

            m_eglContext = EGL_NO_CONTEXT;
        }

        if( m_eglSurface != EGL_NO_SURFACE )
        {
            if( ::eglDestroySurface( m_eglDisplay, m_eglSurface ) == EGL_FALSE )
            {
                LOGGER_WARNING( "[egl] failed to destroy surface [%d]"
                    , ::eglGetError()
                );
            }

            m_eglSurface = EGL_NO_SURFACE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceChangedEvent_( const PlatformUnionEvent::PlatformSurfaceChangedEvent & _event )
    {
        jint surfaceWidth = _event.surfaceWidth;
        jint surfaceHeight = _event.surfaceHeight;

        Resolution windowResolution( surfaceWidth, surfaceHeight );

        APPLICATION_SERVICE()
            ->setWindowResolution( windowResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::clipboardChangedEvent_( const PlatformUnionEvent::PlatformClipboardChangedEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::windowFocusChangedEvent_( const PlatformUnionEvent::PlatformWindowFocusChangedEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::lowMemoryEvent_( const PlatformUnionEvent::PlatformLowMemoryEvent & _event )
    {
        MENGINE_UNUSED( _event );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::trimMemoryEvent_( const PlatformUnionEvent::PlatformTrimMemoryEvent & _event )
    {
        jint level = _event.level;

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_TRIM_MEMORY, level );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::changeLocaleEvent_( const PlatformUnionEvent::PlatformChangeLocale & _event )
    {
        MENGINE_UNUSED( _event );

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pushEvent( const PlatformUnionEvent & _event )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_eventsMutex );

        m_eventsAux.emplace_back( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasEvent( PlatformUnionEvent::EPlatformEventType _type ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_eventsMutex );

        for( const PlatformUnionEvent & event : m_eventsAux )
        {
            if( event.type == _type )
            {
                return true;
            }
        }

        for( const PlatformUnionEvent & event : m_events )
        {
            if( event.type == _type )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
