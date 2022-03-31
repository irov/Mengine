#include "SDLPlatform.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#if defined(MENGINE_PLATFORM_APPLE)
extern "C" {
#   include "Apple/AppleOpenUrlInDefaultBrowser.h"
}
#elif defined(MENGINE_PLATFORM_OSX)
extern "C" {
#   include "OSX/OSXSetDesktopWallpaper.h"
}
#elif defined(MENGINE_PLATFORM_ANDROID)
#   include "Android/AndroidAssetFile.h"
#   include "Android/AndroidOpenUrlInDefaultBrowser.h"
#   include "Android/AndroidOpenMail.h"
#endif

#include "SDLDynamicLibrary.h"
#include "SDLDateTimeProvider.h"

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
#include "Kernel/Document.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/RandomDevice.h"
#include "Kernel/OptionHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/Algorithm.h"
#include "Config/Utils.h"

#if defined(MENGINE_PLATFORM_OSX) || defined(MENGINE_PLATFORM_IOS)
#   include "TargetConditionals.h"
#endif

#if defined(MENGINE_PLATFORM_OSX)
#   include <sysdir.h>
#endif

#include <clocale>
#include <ctime>
#include <iomanip>

#include <sys/stat.h>

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
#if defined(MENGINE_PLATFORM_IOS)
#   ifndef SDL_IPHONE_MAX_GFORCE
#   define SDL_IPHONE_MAX_GFORCE 5.0f
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( SDLFileGroup );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Mengine::SDLPlatform );
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
//////////////////////////////////////////////////////////////////////////
static jclass jclass_activity;
static jobject jobject_activity;
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1setMengineAndroidActivityJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        jclass_activity = (jclass)(env->NewGlobalRef( cls ));
        jobject_activity = (jclass)(env->NewGlobalRef( obj ));
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1quitMengineAndroidActivityJNI )(JNIEnv * env, jclass cls)
    {
        PLATFORM_SERVICE()
            ->closeWindow();
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getCompanyName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getCompanyName( companyName );

        jstring result = env->NewStringUTF( companyName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getProjectName )(JNIEnv * env, jclass cls)
    {
        Mengine::Char projectName[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};
        APPLICATION_SERVICE()
            ->getProjectName( projectName );

        jstring result = env->NewStringUTF( projectName );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getProjectVersion )(JNIEnv * env, jclass cls)
    {
        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        jint result = (jint)projectVersion;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isDebugMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDebugMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isDevelopmentMode )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isDevelopmentMode();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isBuildMaster )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isBuildMaster();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1isBuildPublish )(JNIEnv * env, jclass cls)
    {
        bool mode = Mengine::Helper::isBuildPublish();

        jboolean result = (jboolean)mode;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getEngineGITSHA1 )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * ENGINE_GIT_SHA1 = Mengine::Helper::getEngineGITSHA1();

        jstring result = env->NewStringUTF( ENGINE_GIT_SHA1 );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildTimestamp )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_TIMESTAMP = Mengine::Helper::getBuildTimestamp();

        jstring result = env->NewStringUTF( BUILD_TIMESTAMP );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildUsername )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_USERNAME = Mengine::Helper::getBuildUsername();

        jstring result = env->NewStringUTF( BUILD_USERNAME );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getBuildVersion )(JNIEnv * env, jclass cls)
    {
        const Mengine::Char * BUILD_VERSION = Mengine::Helper::getBuildVersion();

        jstring result = env->NewStringUTF( BUILD_VERSION );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jstring JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeMengine_1getConfigValue )(JNIEnv * env, jclass cls, jstring _section, jstring _key, jstring _default)
    {
        const Mengine::ConfigInterfacePtr & defaultConfig = CONFIG_SERVICE()
            ->getDefaultConfig();

        const Mengine::Char * section_str = env->GetStringUTFChars( _section, nullptr );
        const Mengine::Char * key_str = env->GetStringUTFChars( _key, nullptr );
        const Mengine::Char * default_str = env->GetStringUTFChars( _default, nullptr );

        const Mengine::Char * value_str;
        defaultConfig->hasValue( section_str, key_str, default_str, &value_str );

        env->ReleaseStringUTFChars( _section, section_str );
        env->ReleaseStringUTFChars( _key, key_str );
        env->ReleaseStringUTFChars( _default, default_str );

        jstring result = env->NewStringUTF( value_str );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jboolean JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeKernel_1hasPlugin )(JNIEnv * env, jclass cls, jstring name)
    {
        const Mengine::Char * name_str = env->GetStringUTFChars( name, nullptr );

        bool exist = PLUGIN_SERVICE()
            ->hasPlugin( name_str );

        env->ReleaseStringUTFChars( name, name_str );

        return (jboolean)exist;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::SDLPlatform()
        : m_sdlWindow( nullptr )
        , m_sdlAccelerometer( nullptr )
#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
        , m_glContext( nullptr )
#endif
        , m_sdlInput( nullptr )
        , m_prevTime( 0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_sleepMode( true )
        , m_shouldQuit( false )
        , m_desktop( false )
        , m_touchpad( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::~SDLPlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t SDLPlatform::getCurrentPath( Char * const _currentPath ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH - 1, unicode_path );

        if( len == 0 )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        unicode_path[len] = MENGINE_PATH_WDELIM;
        unicode_path[len + 1] = L'\0';

        Helper::pathCorrectBackslashToW( unicode_path, unicode_path );

        size_t pathLen;
        if( Helper::unicodeToUtf8( unicode_path, _currentPath, MENGINE_MAX_PATH, &pathLen ) == false )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        return pathLen;
#elif defined(MENGINE_PLATFORM_IOS)
        const char deploy_mac_data[] = "";
        MENGINE_STRCPY( _currentPath, deploy_mac_data );

        return sizeof( deploy_mac_data ) - 1;
#elif defined(MENGINE_PLATFORM_OSX)
        char * basePath = SDL_GetBasePath();

        MENGINE_STRCPY( _currentPath, basePath );

        SDL_free( basePath );

        return MENGINE_STRLEN( _currentPath );
#elif defined(MENGINE_PLATFORM_ANDROID)
        _currentPath[0] = L'\0';

        return 0;
#else  
        _currentPath[0] = L'\0';

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getUserPath( Char * const _userPath ) const
    {
#ifdef MENGINE_PLATFORM_WINDOWS
        bool developmentMode = Helper::isDevelopmentMode();
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( (developmentMode == true && roamingMode == false) || noroamingMode == true )
        {
            Char currentPath[MENGINE_MAX_PATH] = {'\0'};
            size_t currentPathLen = this->getCurrentPath( currentPath );

            if( MENGINE_MAX_PATH <= currentPathLen + 5 )
            {
                return 0;
            }

            MENGINE_STRCPY( _userPath, currentPath );
            MENGINE_STRCAT( _userPath, MENGINE_DEVELOPMENT_USER_FOLDER_NAME );
            MENGINE_STRCAT( _userPath, "/" );

            size_t pathLen = MENGINE_STRLEN( _userPath );

            return pathLen;
        }
#endif

        const Char * Project_Company = CONFIG_VALUE( "Project", "Company", "NONAME" );
        const Char * Project_Name = CONFIG_VALUE( "Project", "Name", "UNKNOWN" );

        Char * sdl_prefPath = SDL_GetPrefPath( Project_Company, Project_Name );

        size_t sdl_prefPathLen = MENGINE_STRLEN( sdl_prefPath );

        if( sdl_prefPathLen >= MENGINE_MAX_PATH )
        {
            SDL_free( sdl_prefPath );

            return 0;
        }

        Helper::pathCorrectBackslashToA( _userPath, sdl_prefPath );

        SDL_free( sdl_prefPath );

        return sdl_prefPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getUserName( Char * const _userName ) const
    {
        _userName[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    float SDLPlatform::getJoystickAxis( uint32_t _index ) const
    {
        if( m_sdlAccelerometer == nullptr )
        {
            return 0.f;
        }

        float axis = SDL_JoystickGetAxis( m_sdlAccelerometer, _index );

        const float inv_maxint32f = 1.f / 32767.f;

        axis *= inv_maxint32f;

#if defined(MENGINE_PLATFORM_IOS)
        axis *= SDL_IPHONE_MAX_GFORCE;
#endif
        return axis;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getMaxClientResolution( Resolution * const _resolution ) const
    {
        SDL_Rect rect;
        if( SDL_GetDisplayUsableBounds( 0, &rect ) != 0 )
        {
            LOGGER_ERROR( "invalid get max client resolution: %s"
                , SDL_GetError()
            );

            return false;
        }

        LOGGER_MESSAGE( "client resolution width %d height %d"
            , rect.w
            , rect.h
        );

        *_resolution = Resolution( (uint32_t)rect.w, (uint32_t)rect.h );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int SDL_EventFilter_RemoveMouse( void * userdata, SDL_Event * event )
        {
            MENGINE_UNUSED( userdata );

            switch( event->type )
            {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    return 0;
                }break;
            default: break;
            };

            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        static int SDL_EventFilter_EnterBackground( void * userdata, SDL_Event * event )
        {
            MENGINE_UNUSED( userdata );

            switch( event->type )
            {
            case SDL_APP_WILLENTERBACKGROUND:
                {
                    return 0;
                }break;
            case SDL_APP_DIDENTERBACKGROUND:
                {
                    return 0;
                }break;
            default: break;
            };

            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * SDL_GetCategoryName( int category )
        {
            switch( category )
            {
            case SDL_LOG_CATEGORY_APPLICATION:
                return "application";
            case SDL_LOG_CATEGORY_ERROR:
                return "error";
            case SDL_LOG_CATEGORY_ASSERT:
                return "assert";
            case SDL_LOG_CATEGORY_SYSTEM:
                return "system";
            case SDL_LOG_CATEGORY_AUDIO:
                return "audio";
            case SDL_LOG_CATEGORY_VIDEO:
                return "video";
            case SDL_LOG_CATEGORY_RENDER:
                return "render";
            case SDL_LOG_CATEGORY_INPUT:
                return "input";
            case SDL_LOG_CATEGORY_TEST:
                return "test";
            case SDL_LOG_CATEGORY_CUSTOM:
                return "custom";
            default:
                break;
            }

            return "unknown";
        }
        //////////////////////////////////////////////////////////////////////////
        static ELoggerLevel SDL_GetLoggerLevel( SDL_LogPriority priority )
        {
            switch( priority )
            {
            case SDL_LOG_PRIORITY_VERBOSE:
                return LM_VERBOSE;
            case SDL_LOG_PRIORITY_DEBUG:
                return LM_WARNING;
            case SDL_LOG_PRIORITY_INFO:
                return LM_INFO;
            case SDL_LOG_PRIORITY_WARN:
                return LM_WARNING;
            case SDL_LOG_PRIORITY_ERROR:
                return LM_ERROR;
            case SDL_LOG_PRIORITY_CRITICAL:
                return LM_CRITICAL;
            default:
                break;
            }

            return LM_ERROR;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL_LogOutputFunction( void * userdata, int category, SDL_LogPriority priority, const char * message )
        {
            MENGINE_UNUSED( userdata );
            MENGINE_UNUSED( category );
            MENGINE_UNUSED( priority );

            ELoggerLevel level = Detail::SDL_GetLoggerLevel( priority );

            LOGGER_VERBOSE_LEVEL( ConstString::none(), level, LFILTER_NONE, LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )("SDL [%s]: %s"
                , Detail::SDL_GetCategoryName( category )
                , message
                );
        }
        //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WINDOWS_UNIVERSAL
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_malloc_func( size_t size )
        {
            void * p = Helper::allocateMemory( size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_calloc_func( size_t nmemb, size_t size )
        {
            void * p = Helper::callocateMemory( nmemb, size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * SDL_realloc_func( void * mem, size_t size )
        {
            void * p = Helper::reallocateMemory( mem, size, "SDL" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL_free_func( void * mem )
        {
            Helper::deallocateMemory( mem, "SDL" );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::_initializeService()
    {
#if MENGINE_SETLOCALE_ENABLE
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        SDL_version ver;
        SDL_GetVersion( &ver );

        LOGGER_MESSAGE_RELEASE( "SDL version %u.%u.%u"
            , ver.major
            , ver.minor
            , ver.patch
        );

        SDL_bool isTablet = SDL_IsTablet();

        LOGGER_MESSAGE_RELEASE( "SDL Tablet: %s"
            , isTablet == SDL_TRUE ? "true" : "false"
        );

#ifndef MENGINE_WINDOWS_UNIVERSAL
        SDL_GetMemoryFunctions( &m_old_SDL_malloc_func, &m_old_SDL_calloc_func, &m_old_SDL_realloc_func, &m_old_SDL_free_func );

        if( SDL_SetMemoryFunctions( &Detail::SDL_malloc_func, &Detail::SDL_calloc_func, &Detail::SDL_realloc_func, &Detail::SDL_free_func ) != 0 )
        {
            LOGGER_ERROR( "invalid set memory functions: %s"
                , SDL_GetError()
            );

            return false;
        }
#endif

#ifdef MENGINE_DEBUG
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_DEBUG );
#else
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_ERROR );
#endif

        SDL_LogSetOutputFunction( &Detail::SDL_LogOutputFunction, nullptr );

        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) != 0 )
        {
            LOGGER_ERROR( "invalid initialize error: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_platformTags.clear();

        const Char * sdlPlatform = SDL_GetPlatform();

        if( MENGINE_STRCMP( sdlPlatform, "Windows" ) == 0 )
        {
            m_desktop = true;
            m_touchpad = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );

#ifdef MENGINE_PLATFORM_WINDOWS32
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#ifdef MENGINE_PLATFORM_WINDOWS64
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif
        }
        else if( MENGINE_STRCMP( sdlPlatform, "WinRT" ) == 0 )
        {
            m_desktop = true;
            m_touchpad = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WinRT" ) );


            SDL_SetEventFilter( &Detail::SDL_EventFilter_EnterBackground, nullptr );
        }
        else if( MENGINE_STRCMP( sdlPlatform, "Mac OS X" ) == 0 )
        {
            m_desktop = true;
            m_touchpad = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );
        }
        else if( MENGINE_STRCMP( sdlPlatform, "Android" ) == 0 )
        {
            m_desktop = false;
            m_touchpad = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

            if( isTablet == SDL_TRUE )
            {
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "TABLET" ) );
            }
            else
            {
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PHONE" ) );
            }

            SDL_SetEventFilter( &Detail::SDL_EventFilter_RemoveMouse, nullptr );
        }
        else if( MENGINE_STRCMP( sdlPlatform, "iOS" ) == 0 )
        {
            m_desktop = false;
            m_touchpad = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );

            if( isTablet == SDL_TRUE )
            {
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "TABLET" ) );
            }
            else
            {
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PHONE" ) );
            }

            SDL_SetEventFilter( &Detail::SDL_EventFilter_RemoveMouse, nullptr );
        }
        else if( MENGINE_STRCMP( sdlPlatform, "Linux" ) == 0 )
        {
            m_desktop = true;
            m_touchpad = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "LINUX" ) );
        }
        else
        {
            LOGGER_ERROR( "platform '%s' unspecified"
                , sdlPlatform
            );
        }

        if( HAS_OPTION( "win32" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
        }
        else if( HAS_OPTION( "win64" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
        }
        else if( HAS_OPTION( "mac" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );
        }
        else if( HAS_OPTION( "ios" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );
        }
        else if( HAS_OPTION( "android" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );
        }
        else if( HAS_OPTION( "linux" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "LINUX" ) );
        }
        else if( HAS_OPTION( "wp" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.clear();
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WP" ) );
        }

        if( HAS_OPTION( "table" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "TABLET" ) );
        }
        else if( HAS_OPTION( "phone" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PHONE" ) );
        }

        const Char * option_platform = GET_OPTION_VALUE( "platform", nullptr );

        if( option_platform != nullptr )
        {
            Char uppercase_option_platform[256] = {'\0'};
            Helper::toupper( option_platform, uppercase_option_platform, 255 );

            m_platformTags.clear();
            m_platformTags.addTag( Helper::stringizeString( option_platform ) );
        }

        LOGGER_MESSAGE_RELEASE( "Platform Tags: %s"
            , [this]()
        {
            static Char cache_value[1024] = {'\0'};

            for( const ConstString & tag : m_platformTags.getValues() )
            {
                MENGINE_STRCAT( cache_value, tag.c_str() );
                MENGINE_STRCAT( cache_value, "-" );
            }

            return cache_value;
        }() );

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        LOGGER_MESSAGE_RELEASE( "Touchpad: %u"
            , m_touchpad
        );

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        LOGGER_MESSAGE_RELEASE( "Desktop: %u"
            , m_desktop
        );

        SDLInputPtr sdlInput = Helper::makeFactorableUnique<SDLInput>( MENGINE_DOCUMENT_FACTORABLE );

        if( sdlInput->initialize() == false )
        {
            return false;
        }

        m_sdlInput = sdlInput;

        m_factoryDynamicLibraries = Helper::makeFactoryPool<SDLDynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDateTimeProviders = Helper::makeFactoryPool<SDLDateTimeProvider, 8>( MENGINE_DOCUMENT_FACTORABLE );


#if defined(MENGINE_PLATFORM_ANDROID)
        int AndroidSDKVersion = SDL_GetAndroidSDKVersion();
        SDL_bool AndroidTV = SDL_IsAndroidTV();

        LOGGER_MESSAGE_RELEASE( "Android SDK version: %d", AndroidSDKVersion );
        LOGGER_MESSAGE_RELEASE( "Android TV: %d", AndroidTV );
#endif

        SERVICE_WAIT( FileServiceInterface, [this]()
        {
            if( this->initializeFileService_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_LEAVE( FileServiceInterface, [this]()
        {
            this->finalizeFileService_();
        } );

#if defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * env = Mengine_JNI_GetEnv();

        m_jenv = env;
#endif

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES, &SDLPlatform::notifyInitializeBaseServices_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &SDLPlatform::notifyCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );

        uint32_t deviceSeed = Helper::generateRandomDeviceSeed();

        LOGGER_MESSAGE_RELEASE( "Device Seed: %u"
            , deviceSeed
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::_runService()
    {
        DateTimeProviderInterfacePtr dateTimeProvider =
            this->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        LOGGER_MESSAGE_RELEASE( "Date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        LOGGER_MESSAGE_RELEASE( "[Device Info]" );

        LOGGER_MESSAGE_RELEASE( "Platform: %s"
            , SDL_GetPlatform()
        );

        LOGGER_MESSAGE_RELEASE( "CPU: %d Count %d CacheLineSize"
            , SDL_GetCPUCount()
            , SDL_GetCPUCacheLineSize()
        );

        LOGGER_MESSAGE_RELEASE( "CPU RDTSC: %d"
            , SDL_HasRDTSC()
        );

        LOGGER_MESSAGE_RELEASE( "CPU AltiVec: %d"
            , SDL_HasAltiVec()
        );

        LOGGER_MESSAGE_RELEASE( "CPU MMX: %d"
            , SDL_HasMMX()
        );

        LOGGER_MESSAGE_RELEASE( "CPU 3DNow: %d"
            , SDL_Has3DNow()
        );

        LOGGER_MESSAGE_RELEASE( "CPU SSE: %d"
            , SDL_HasSSE()
        );

        LOGGER_MESSAGE_RELEASE( "CPU SSE2: %d"
            , SDL_HasSSE2()
        );

        LOGGER_MESSAGE_RELEASE( "CPU SSE3: %d"
            , SDL_HasSSE3()
        );

        LOGGER_MESSAGE_RELEASE( "CPU SSE41: %d"
            , SDL_HasSSE41()
        );

        LOGGER_MESSAGE_RELEASE( "CPU SSE42: %d"
            , SDL_HasSSE42()
        );

        LOGGER_MESSAGE_RELEASE( "CPU AVX: %d"
            , SDL_HasAVX()
        );

        LOGGER_MESSAGE_RELEASE( "CPU AVX2: %d"
            , SDL_HasAVX2()
        );

        LOGGER_MESSAGE_RELEASE( "CPU AVX512F: %d"
            , SDL_HasAVX512F()
        );

        LOGGER_MESSAGE_RELEASE( "CPU NEON: %d"
            , SDL_HasNEON()
        );

        LOGGER_MESSAGE_RELEASE( "RAM: %d MB"
            , SDL_GetSystemRAM()
        );

        // Search accelerometer device among joysticks
        int numJoysticks = SDL_NumJoysticks();

        for( int deviceIndex = 0; deviceIndex != numJoysticks; ++deviceIndex )
        {
            SDL_Joystick * joystick = SDL_JoystickOpen( deviceIndex );

            if( joystick == nullptr )
            {
                continue;
            }

            const Char * joystickName = SDL_JoystickName( joystick );

            bool isAccelerometer = false;

            if( MENGINE_STRCMP( joystickName, "Android Accelerometer" ) == 0 )
            {
                isAccelerometer = true;
            }
            else if( MENGINE_STRCMP( joystickName, "iOS Accelerometer" ) == 0 )
            {
                isAccelerometer = true;
            }

            if( isAccelerometer == true )
            {
                LOGGER_MESSAGE_RELEASE( "Accelerometer found: %s"
                    , joystickName
                );

                m_sdlAccelerometer = joystick;

                break;
            }
        }

        if( m_sdlAccelerometer == nullptr )
        {
            LOGGER_MESSAGE_RELEASE( "Accelerometer not found" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::_stopService()
    {
        if( m_sdlAccelerometer != nullptr )
        {
            if( SDL_JoystickGetAttached( m_sdlAccelerometer ) == SDL_TRUE )
            {
                SDL_JoystickClose( m_sdlAccelerometer );
            }

            m_sdlAccelerometer = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        m_active = false;

#ifdef MENGINE_DEBUG
        for( const TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ERROR( "forgot remove platform timer (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timers.clear();

#ifdef MENGINE_DEBUG
        for( const UpdateDesc & desc : m_updates )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ERROR( "forgot remove platform update (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_updates.clear();

        this->destroyWindow_();

        SDL_Quit();

        if( m_sdlInput != nullptr )
        {
            m_sdlInput->finalize();
            m_sdlInput = nullptr;
        }

        m_platformTags.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDateTimeProviders );

        m_factoryDynamicLibraries = nullptr;
        m_factoryDateTimeProviders = nullptr;

#ifndef MENGINE_WINDOWS_UNIVERSAL
        if( SDL_SetMemoryFunctions( m_old_SDL_malloc_func, m_old_SDL_calloc_func, m_old_SDL_realloc_func, m_old_SDL_free_func ) != 0 )
        {
            LOGGER_ERROR( "invalid set memory functions: %s"
                , SDL_GetError()
            );
        }
#endif

        MENGINE_ASSERTION_ALLOCATOR( "SDL" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::runPlatform()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::tickPlatform( float _frameTime )
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        for( const UpdateDesc & desc : m_updates )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.lambda( desc.id );
        }

        m_updates.erase( Algorithm::remove_if( m_updates.begin(), m_updates.end(), []( const UpdateDesc & _desc )
        {
            return _desc.id == INVALID_UNIQUE_ID;
        } ), m_updates.end() );

        for( TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.time -= _frameTime;

            if( desc.time > 0.f )
            {
                continue;
            }

            desc.time += desc.milliseconds;

            desc.lambda( desc.id );
        }

        m_timers.erase( Algorithm::remove_if( m_timers.begin(), m_timers.end(), []( const TimerDesc & _desc )
        {
            return _desc.id == INVALID_UNIQUE_ID;
        } ), m_timers.end() );

        bool updating = APPLICATION_SERVICE()
            ->beginUpdate();

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

        if( this->isNeedWindowRender() == true )
        {
            bool sucessful = APPLICATION_SERVICE()
                ->render();

            if( sucessful == true )
            {
                APPLICATION_SERVICE()
                    ->flush();

                if( m_sdlWindow != nullptr )
                {
                    SDL_ShowWindow( m_sdlWindow );

                    if( SDL_GetWindowFlags( m_sdlWindow ) & SDL_WINDOW_OPENGL )
                    {
                        SDL_GL_SwapWindow( m_sdlWindow );
                    }
                }
            }
        }

        APPLICATION_SERVICE()
            ->endUpdate();

        if( updating == false )
        {
            if( m_pauseUpdatingTime < 0.f )
            {
                m_pauseUpdatingTime = _frameTime;
            }

            if( m_sleepMode == true )
            {
                SDL_Delay( 100 );
            }
            else
            {
                SDL_Delay( 1 );
            }
        }
        else
        {
#if defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_OSX)
            bool maxfps = HAS_OPTION( "maxfps" );

            if( APPLICATION_SERVICE()
                ->getVSync() == false && maxfps == false )
            {
                SDL_Delay( 1 );
            }
#endif
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::updatePlatform()
    {
        this->setActive_( true );

        m_prevTime = TIME_SYSTEM()
            ->getTimeMilliseconds();

        for( ;; )
        {
            uint64_t currentTime = TIME_SYSTEM()
                ->getTimeMilliseconds();

            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            if( this->tickPlatform( frameTime ) == false )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::openUrlInDefaultBrowser( const Char * _url )
    {
        MENGINE_UNUSED( _url );

#if defined(MENGINE_PLATFORM_APPLE)
        if( AppleOpenUrlInDefaultBrowser( _url ) == -1 )
        {
            LOGGER_ERROR( "error open url in default browser '%s'"
                , _url
            );

            return false;
        }
        
        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        if( AndroidOpenUrlInDefaultBrowser( m_jenv, jclass_activity, jobject_activity, _url ) == false )
        {
            LOGGER_ERROR( "error open url in default browser '%s'"
                , _url
            );

            return false;
        }
        
        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        return true;
#else
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );

#if defined(MENGINE_PLATFORM_APPLE)
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#elif defined(MENGINE_PLATFORM_ANDROID)
        if( AndroidOpenMail( m_jenv, jclass_activity, jobject_activity, _email, _subject, _body ) == false )
        {
            LOGGER_ERROR( "error open mail '%s'"
                , _email
            );

            return false;
        }

        return true;
#else
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::stopPlatform()
    {
        m_shouldQuit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId SDLPlatform::addUpdate( const LambdaTimer & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        UpdateDesc desc;
        desc.id = new_id;
        desc.lambda = _lambda;

#ifdef MENGINE_DEBUG
        desc.doc = _doc;
#endif

        m_updates.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::removeUpdate( UniqueId _id )
    {
        VectorUpdates::iterator it_found = Algorithm::find_if( m_updates.begin(), m_updates.end(), [_id]( const UpdateDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_updates.end(), "not found update '%u'"
            , _id
        );

        UpdateDesc & desc = *it_found;

        desc.id = INVALID_UNIQUE_ID;
        desc.lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SDLPlatform::addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        TimerDesc desc;
        desc.id = new_id;
        desc.milliseconds = _milliseconds;
        desc.time = _milliseconds;
        desc.lambda = _lambda;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_timers.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::removeTimer( uint32_t _id )
    {
        VectorTimers::iterator it_found = Algorithm::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_timers.end() );

        TimerDesc & desc = *it_found;

        desc.id = INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setSleepMode( bool _sleepMode )
    {
        m_sleepMode = _sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getSleepMode() const
    {
        return m_sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLPlatform::getTicks() const
    {
        Uint32 ticks = SDL_GetTicks();

        return (uint64_t)ticks;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        m_projectTitle.assign( _projectTitle );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow( const Resolution & _windowResolution, bool _fullscreen )
    {
        LOGGER_MESSAGE( "create window size %u:%u fullscreen %d"
            , _windowResolution.getWidth()
            , _windowResolution.getHeight()
            , _fullscreen
        );

        this->setupWindow_();

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
    bool SDLPlatform::applyWindow_()
    {
#if defined(MENGINE_WINDOWS_UNIVERSAL)
#else
        SDL_GLContext glContext = SDL_GL_CreateContext( m_sdlWindow );

        if( glContext == nullptr )
        {
            LOGGER_ERROR( "invalid create GL context error: %s"
                , SDL_GetError()
            );

            SDL_DestroyWindow( m_sdlWindow );
            m_sdlWindow = nullptr;

            return false;
        }

        int attribute_GL_CONTEXT_PROFILE_MASK = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &attribute_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_CONTEXT_PROFILE_MASK error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MAJOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &attribute_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_CONTEXT_MAJOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MINOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &attribute_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_CONTEXT_MINOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_RED_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &attribute_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_RED_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_GREEN_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &attribute_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_GREEN_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_BLUE_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &attribute_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_BLUE_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_ALPHA_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &attribute_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_ALPHA_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DEPTH_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &attribute_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_DEPTH_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DOUBLEBUFFER = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &attribute_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_ERROR( "get attribute SDL_GL_DOUBLEBUFFER error: %s"
                , SDL_GetError()
            );
        }

        LOGGER_MESSAGE_RELEASE( "SDL_GL_CONTEXT_PROFILE_MASK: %d"
            , attribute_GL_CONTEXT_PROFILE_MASK
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_CONTEXT_MAJOR_VERSION: %d"
            , attribute_GL_CONTEXT_MAJOR_VERSION
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_CONTEXT_MINOR_VERSION: %d"
            , attribute_GL_CONTEXT_MINOR_VERSION
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_RED_SIZE: %d"
            , attribute_SDL_GL_RED_SIZE
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_GREEN_SIZE: %d"
            , attribute_SDL_GL_GREEN_SIZE
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_BLUE_SIZE: %d"
            , attribute_SDL_GL_BLUE_SIZE
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_ALPHA_SIZE: %d"
            , attribute_SDL_GL_ALPHA_SIZE
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_DEPTH_SIZE: %d"
            , attribute_SDL_GL_DEPTH_SIZE
        );

        LOGGER_MESSAGE_RELEASE( "SDL_GL_DOUBLEBUFFER: %d"
            , attribute_SDL_GL_DOUBLEBUFFER
        );

        m_glContext = glContext;

        int drawable_width;
        int drawable_height;
        SDL_GL_GetDrawableSize( m_sdlWindow, &drawable_width, &drawable_height );

        LOGGER_MESSAGE_RELEASE( "SDL drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );
#endif

        int win_width;
        int win_height;
        SDL_GetWindowSize( m_sdlWindow, &win_width, &win_height );

        LOGGER_MESSAGE_RELEASE( "SDL window size [%d, %d]"
            , win_width
            , win_height
        );

#ifdef MENGINE_PLATFORM_DESKTOP
        int win_top;
        int win_left;
        int win_bottom;
        int win_right;
        if( SDL_GetWindowBordersSize( m_sdlWindow, &win_top, &win_left, &win_bottom, &win_right ) == 0 )
        {
            LOGGER_MESSAGE_RELEASE( "SDL window borders [%d, %d] - [%d, %d]"
                , win_left
                , win_top
                , win_right
                , win_bottom
            );
        }
#endif

        int win_min_width;
        int win_min_height;
        SDL_GetWindowMinimumSize( m_sdlWindow, &win_min_width, &win_min_height );

        LOGGER_MESSAGE_RELEASE( "SDL window min size [%d, %d]"
            , win_min_width
            , win_min_height
        );

        int win_max_width;
        int win_max_height;
        SDL_GetWindowMaximumSize( m_sdlWindow, &win_max_width, &win_max_height );

        LOGGER_MESSAGE_RELEASE( "SDL window max size [%d, %d]"
            , win_max_width
            , win_max_height
        );

        int displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );

        if( displayIndex >= 0 )
        {
            SDL_Rect displayBounds;
            if( SDL_GetDisplayBounds( displayIndex, &displayBounds ) == 0 )
            {
                LOGGER_MESSAGE_RELEASE( "SDL display bounds [%d, %d] size [%d, %d]"
                    , displayBounds.x
                    , displayBounds.y
                    , displayBounds.w
                    , displayBounds.h
                );
            }

            SDL_Rect usableBounds;
            if( SDL_GetDisplayUsableBounds( displayIndex, &usableBounds ) == 0 )
            {
                LOGGER_MESSAGE_RELEASE( "SDL display usable bounds [%d, %d] size [%d, %d]"
                    , usableBounds.x
                    , usableBounds.y
                    , usableBounds.w
                    , usableBounds.h
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::attachWindow( const void * _hWND )
    {
        this->setupWindow_();

        SDL_Window * sharePixelFormatWindow = SDL_CreateWindow( "MengineSharePixelFormatWindow", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );

        Char sBuf[64];
        MENGINE_SPRINTF( sBuf, "%p", sharePixelFormatWindow );

        SDL_SetHint( SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, sBuf );

        SDL_Window * sdlWindow = SDL_CreateWindowFrom( _hWND );

        SDL_SetHint( SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, nullptr );

        SDL_DestroyWindow( sharePixelFormatWindow );

        if( sdlWindow == nullptr )
        {
            LOGGER_ERROR( "create window from [%p] failed: %s"
                , _hWND
                , SDL_GetError()
            );

            return false;
        }

        m_sdlWindow = sdlWindow;

        if( this->applyWindow_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & SDLPlatform::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr SDLPlatform::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        SDLDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

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
    bool SDLPlatform::getDesktopResolution( Resolution * const _resolution ) const
    {
#if !defined(MENGINE_PLATFORM_IOS)
        int displayIndex;

        if( m_sdlWindow == nullptr )
        {
            displayIndex = 0;
        }
        else
        {
            displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );
        }
#endif

        uint32_t width;
        uint32_t height;

#if defined(MENGINE_PLATFORM_IOS)
        int drawable_width;
        int drawable_height;
        SDL_GL_GetDrawableSize( m_sdlWindow, &drawable_width, &drawable_height );

        width = (uint32_t)drawable_width;
        height = (uint32_t)drawable_height;
#elif defined(MENGINE_PLATFORM_ANDROID)
        SDL_Rect rect;
        if( SDL_GetDisplayBounds( displayIndex, &rect ) != 0 )
        {
            LOGGER_ERROR( "invalid get desktop resolution error: %s"
                , SDL_GetError()
            );

            return false;
        }

        width = (uint32_t)rect.w;
        height = (uint32_t)rect.h;
#else
        SDL_DisplayMode dm;
        if( SDL_GetDesktopDisplayMode( displayIndex, &dm ) != 0 )
        {
            LOGGER_ERROR( "invalid get desktop resolution error: %s"
                , SDL_GetError()
            );

            return false;
        }

        width = (uint32_t)dm.w;
        height = (uint32_t)dm.h;
#endif

        * _resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getAlwaysFullscreen() const
    {
#if defined(MENGINE_PLATFORM_IOS)
        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        return true;
#endif

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::setProcessDPIAware()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::isDebuggerPresent() const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
#else

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full )
    {
        MENGINE_UNUSED( _dumpPath );
        MENGINE_UNUSED( _pExceptionPointers );
        MENGINE_UNUSED( _full );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::closeWindow()
    {
        m_shouldQuit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::minimizeWindow()
    {
        SDL_MinimizeWindow( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setCursorPosition( const mt::vec2f & _pos )
    {
        Resolution resolution;
        if( this->getDesktopResolution( &resolution ) == false )
        {
            return;
        }

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        int wndPosX = static_cast<int>(_pos.x * width);
        int wndPosY = static_cast<int>(_pos.y * height);

        // ! This function generates a mouse motion event !
        SDL_WarpMouseInWindow( m_sdlWindow, wndPosX, wndPosY );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::showKeyboard()
    {
        SDL_StartTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::hideKeyboard()
    {
        SDL_StopTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::isShowKeyboard() const
    {
        SDL_bool active = SDL_IsTextInputActive();

        return active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_sdlWindow == nullptr )
        {
            return true;
        }

        Uint32 flags = SDL_GetWindowFlags( m_sdlWindow );

        bool alredyFullscreen = (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;

        if( _fullscreen != alredyFullscreen )
        {
            if( this->changeWindow_( _resolution, _fullscreen ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        if( SDL_GetWindowFlags( m_sdlWindow ) & SDL_WINDOW_OPENGL )
        {
            int profile;
            if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &profile ) == -1 )
            {
                return;
            }

            if( profile == SDL_GL_CONTEXT_PROFILE_ES )
            {
                return;
            }

            if( _vsync == false )
            {
                if( SDL_GL_SetSwapInterval( 0 ) != 0 )
                {
                    LOGGER_ERROR( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                if( SDL_GL_SetSwapInterval( 1 ) != 0 )
                {
                    LOGGER_ERROR( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyCursorModeChanged( bool _mode )
    {
        if( _mode == true )
        {
            SDL_ShowCursor( SDL_ENABLE );
        }
        else
        {
            SDL_ShowCursor( SDL_DISABLE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::onEvent( const ConstString & _event, const MapWParams & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_createDurectoryFullpath( const Char * _fullpath )
        {
#if defined(MENGINE_PLATFORM_IOS)
            int status = ::mkdir( _fullpath, 0700 );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' alredy exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_OSX)
            int status = ::mkdir( _fullpath, 0700 );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' alredy exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_LINUX)
            int status = ::mkdir( _fullpath, 0700 );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' alredy exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_ANDROID)
            int status = ::mkdir( _fullpath, 0700 );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' alredy exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_WINDOWS)
            WChar unicode_fullpath[MENGINE_MAX_PATH] = {L'\0'};
            Helper::utf8ToUnicode( _fullpath, unicode_fullpath, MENGINE_MAX_PATH - 1 );

            BOOL successful = ::CreateDirectoryW( unicode_fullpath, NULL );

            if( successful == FALSE )
            {
                DWORD err = GetLastError();

                switch( err )
                {
                case ERROR_ALREADY_EXISTS:
                    {
                        return true;
                    }break;
                case ERROR_PATH_NOT_FOUND:
                    {
                        LOGGER_WARNING( "'%s' not found"
                            , _fullpath
                        );

                        return false;
                    }break;
                default:
                    {
                        LOGGER_WARNING( "'%s' unknown error %d"
                            , _fullpath
                            , err
                        );
                    }break;
                }

                return false;
            }
#endif

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isDirectoryFullpath( const Char * _fullpath )
        {
            struct stat sb;
            int err = stat( _fullpath, &sb );

            if( err != 0 )
            {
                return false;
            }

            if( (sb.st_mode & S_IFMT) == S_IFDIR )
            {
                return true;
            }

            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::existDirectory( const Char * _path, const Char * _directory ) const
    {
        Char pathDirectory[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        size_t len = MENGINE_STRLEN( pathDirectory );

        if( len == 0 )	// current dir
        {
            return true;
        }

        Char pathFull[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( pathFull, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

        if( pathFull[len - 1] == ':' )	// root dir
        {
            return true;	// let it be
        }
#if defined(MENGINE_PLATFORM_OSX)
        else if( pathFull[len - 1] == '~' )	// root dir
        {
            return true;	// let it be
        }
#endif

        bool exist = Detail::s_isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectory( const Char * _path, const Char * _directory )
    {
        size_t directorySize = MENGINE_STRLEN( _directory );

        if( directorySize == 0 )
        {
            return true;
        }

        Char pathDirectory[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        if( MENGINE_STRLEN( pathDirectory ) == 0 )
        {
            return true;
        }

        Char pathTestDirectory[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( pathTestDirectory, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

        if( Detail::s_isDirectoryFullpath( pathTestDirectory ) == true )
        {
            return true;
        }

        Helper::pathRemoveBackslashA( pathDirectory );

        VectorString paths;

        for( ;; )
        {
            paths.push_back( pathDirectory );

            if( Helper::pathRemoveFileSpecA( pathDirectory ) == false )
            {
                break;
            }

            if( MENGINE_STRLEN( _path ) == 0 )
            {
                break;
            }

            Helper::pathRemoveBackslashA( pathDirectory );

            MENGINE_SNPRINTF( pathTestDirectory, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

            if( Detail::s_isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( VectorString::reverse_iterator
            it = paths.rbegin(),
            it_end = paths.rend();
            it != it_end;
            ++it )
        {
            const String & path = *it;

            const Char * path_str = path.c_str();

            Char pathCreateDirectory[MENGINE_MAX_PATH] = {'\0'};
            MENGINE_SNPRINTF( pathCreateDirectory, MENGINE_MAX_PATH, "%s%s", _path, path_str );

            if( Detail::s_createDurectoryFullpath( pathCreateDirectory ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::existFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        SDL_SuppressError( SDL_TRUE );

        SDL_RWops * rwops = SDL_RWFromFile( pathCorrect, "rb" );

        SDL_SuppressError( SDL_FALSE );

        if( rwops == nullptr )
        {
            return false;
        }

        if( SDL_RWclose( rwops ) != 0 )
        {
            LOGGER_ERROR( "invalid close '%s' get error: %s"
                , pathCorrect
                , SDL_GetError()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::removeFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        int result = ::remove( pathCorrect );

        if( result != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        Char oldPathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Char newPathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

        struct stat sb;
        if( stat( newPathCorrect, &sb ) == 0 && ((sb.st_mode) & S_IFMT) != S_IFDIR )
        {
            int result_remove = ::remove( newPathCorrect );

            if( result_remove != 0 )
            {
                const char * msg = ::strerror( errno );

                LOGGER_ERROR( "invalid remove new move file from '%s' to '%s' error '%s' [%u]"
                    , _oldFilePath
                    , _newFilePath
                    , msg
                    , errno
                );
            }
        }

        int result_rename = ::rename( oldPathCorrect, newPathCorrect );

        if( result_rename != 0 )
        {
            const char * msg = ::strerror( errno );

            LOGGER_ERROR( "invalid move file from '%s' to '%s' error '%s' [%u]"
                , _oldFilePath
                , _newFilePath
                , msg
                , errno
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static bool listDirectoryContents( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaFilePath & _lambda, bool * const _stop )
        {
            {
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, _mask );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind != INVALID_HANDLE_VALUE )
                {
                    do
                    {
                        if( ::wcscmp( fdFile.cFileName, L"." ) == 0
                            || ::wcscmp( fdFile.cFileName, L".." ) == 0 )
                        {
                            continue;
                        }

                        if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                        {
                            continue;
                        }

                        WChar sPath2[MENGINE_MAX_PATH] = {L'\0'};
                        MENGINE_WCSCPY( sPath2, sPath );
                        MENGINE_WCSCAT( sPath2, L"\0" );

                        Helper::pathCorrectForwardslashW( sPath2 );

                        ::PathRemoveFileSpec( sPath2 );

                        WChar unicode_filepath[MENGINE_MAX_PATH] = {L'\0'};
                        ::PathCombine( unicode_filepath, sPath2, fdFile.cFileName );

                        WChar unicode_out[MENGINE_MAX_PATH] = {L'\0'};
                        if( MENGINE_WCSLEN( _dir ) != 0 )
                        {
                            ::PathRelativePathTo( unicode_out,
                                _dir,
                                FILE_ATTRIBUTE_DIRECTORY,
                                unicode_filepath,
                                FILE_ATTRIBUTE_NORMAL );
                        }
                        else
                        {
                            MENGINE_WCSCPY( unicode_out, unicode_filepath );
                        }

                        Char utf8_filepath[MENGINE_MAX_PATH] = {'\0'};
                        if( Helper::unicodeToUtf8( unicode_out, utf8_filepath, MENGINE_MAX_PATH ) == false )
                        {
                            ::FindClose( hFind );

                            return false;
                        }

                        FilePath fp = Helper::stringizeFilePath( utf8_filepath );

                        if( _lambda( fp ) == false )
                        {
                            ::FindClose( hFind );

                            *_stop = true;

                            return true;
                        }

                    } while( ::FindNextFile( hFind, &fdFile ) != FALSE );
                }

                ::FindClose( hFind );
            }

            {
                WChar sPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WCSCPY( sPath, _dir );
                MENGINE_WCSCAT( sPath, _path );
                MENGINE_WCSCAT( sPath, L"*.*" );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind == INVALID_HANDLE_VALUE )
                {
                    return true;
                }

                do
                {
                    if( MENGINE_WCSCMP( fdFile.cFileName, L"." ) == 0
                        || MENGINE_WCSCMP( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                    {
                        continue;
                    }

                    WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
                    MENGINE_WCSCPY( currentPath, sPath );
                    MENGINE_WCSCAT( currentPath, L"\0" );

                    ::PathRemoveFileSpec( currentPath );

                    WChar nextPath[MENGINE_MAX_PATH] = {L'\0'};
                    ::PathCombine( nextPath, currentPath, fdFile.cFileName );

                    MENGINE_WCSCAT( nextPath, L"\\" );

                    bool stop;
                    if( Detail::listDirectoryContents( _dir, _mask, nextPath, _lambda, &stop ) == false )
                    {
                        ::FindClose( hFind );

                        return false;
                    }

                    if( stop == true )
                    {
                        ::FindClose( hFind );

                        *_stop = true;

                        return true;
                    }

                } while( ::FindNextFile( hFind, &fdFile ) != FALSE );

                ::FindClose( hFind );
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _base );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
        WChar unicode_base[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        WChar unicode_mask[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        bool stop;
        if( Detail::listDirectoryContents( unicode_base, unicode_mask, unicode_path, _lambda, &stop ) == false )
        {
            return false;
        }

        MENGINE_UNUSED( stop );

        return true;
#else
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
    //////////////////////////////////////////////////////////////////////////
    static time_t s_FileTimeToUnixTime( const FILETIME * filetime )
    {
        uint32_t a0;			/* 16 bit, low    bits */
        uint32_t a1;			/* 16 bit, medium bits */
        uint32_t a2;			/* 32 bit, high   bits */

        uint32_t carry;		/* carry bit for subtraction */
        int negative;		/* whether a represents a negative value */

                            /* Copy the time values to a2/a1/a0 */
        a2 = filetime->dwHighDateTime;
        a1 = ((uint32_t)filetime->dwLowDateTime) >> 16;
        a0 = ((uint32_t)filetime->dwLowDateTime) & 0xffff;

        /* Subtract the time difference */
        if( a0 >= 32768 )
            a0 -= 32768, carry = 0;
        else
            a0 += (1 << 16) - 32768, carry = 1;

        if( a1 >= 54590 + carry )
            a1 -= 54590 + carry, carry = 0;
        else
            a1 += (1 << 16) - 54590 - carry, carry = 1;

        a2 -= 27111902 + carry;

        /* If a is negative, replace a by (-1-a) */
        negative = (a2 >= ((uint32_t)1) << 31);
        if( negative )
        {
            /* Set a to -a - 1 (a is a2/a1/a0) */
            a0 = 0xffff - a0;
            a1 = 0xffff - a1;
            a2 = ~a2;
        }

        /* Divide a by 10000000 (a = a2/a1/a0), put the rest into r.
        Split the divisor into 10000 * 1000 which are both less than 0xffff. */
        a1 += (a2 % 10000) << 16;
        a2 /= 10000;
        a0 += (a1 % 10000) << 16;
        a1 /= 10000;
        a0 /= 10000;

        a1 += (a2 % 1000) << 16;
        a2 /= 1000;
        a0 += (a1 % 1000) << 16;
        a1 /= 1000;
        a0 /= 1000;

        /* If a was negative, replace a by (-1-a) and r by (9999999 - r) */
        if( negative )
        {
            /* Set a to -a - 1 (a is a2/a1/a0) */
            a0 = 0xffff - a0;
            a1 = 0xffff - a1;
            a2 = ~a2;
        }

        /* Do not replace this by << 32, it gives a compiler warning and it does
        not work. */
        return ((((time_t)a2) << 16) << 16) + ((time_t)a1 << 16) + a0;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLPlatform::getFileTime( const Char * _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
        WChar unicode_filePath[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH - 1 ) == false )
        {
            return 0U;
        }

        HANDLE handle = ::CreateFile( unicode_filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( handle, &creation, &access, &write ) == FALSE )
        {
            ::CloseHandle( handle );

            return 0U;
        }

        ::CloseHandle( handle );

        time_t time = s_FileTimeToUnixTime( &write );

        return time;
#else
        return 0U;
#endif		
    }
    //////////////////////////////////////////////////////////////////////////
    DateTimeProviderInterfacePtr SDLPlatform::createDateTimeProvider( const DocumentPtr & _doc )
    {
        SDLDateTimeProviderPtr dateTimeProvider = m_factoryDateTimeProviders->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider );

        return dateTimeProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

#if defined(MENGINE_PLATFORM_OSX)
        char * homeBuffer = getenv( "HOME" );

        if( homeBuffer == nullptr )
        {
            LOGGER_ERROR( "invalid get env 'HOME'" );

            return false;
        }

        Char path_pictures[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_pictures, MENGINE_MAX_PATH, "%s/Pictures/", homeBuffer );

        Char path_file[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_file, MENGINE_MAX_PATH, "%s%s%s", path_pictures, _directoryPath, _filePath );

        if( OSXSetDesktopWallpaper( path_file ) == -1 )
        {
            LOGGER_ERROR( "error set desktop wallpaper '%s'"
                , path_file
            );
        }
#endif

        //MENGINE_ASSERTION_NOT_IMPLEMENTED();


        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
#if defined(MENGINE_PLATFORM_OSX)
        char * homeBuffer = getenv( "HOME" );

        if( homeBuffer == nullptr )
        {
            LOGGER_ERROR( "invalid get env 'HOME'" );

            return false;
        }

        Char path_pictures[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_pictures, MENGINE_MAX_PATH, "%s/Pictures/", homeBuffer );

        if( this->createDirectory( path_pictures, _directoryPath ) == false )
        {
            LOGGER_ERROR( "invalid create directory '%s%s'"
                , path_pictures
                , _directoryPath
            );

            return false;
        };

        Char path_file[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_file, MENGINE_MAX_PATH, "%s%s%s", path_pictures, _directoryPath, _filePath );

        SDL_RWops * rwops = SDL_RWFromFile( path_file, "wb" );

        if( rwops == nullptr )
        {
            LOGGER_ERROR( "invalid create file '%s'"
                , path_file
            );

            return false;
        }

        size_t written = SDL_RWwrite( rwops, _data, 1, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size %zu [error: %s]"
                , path_file
                , _size
                , SDL_GetError()
            );

            return false;
        }

        SDL_RWclose( rwops );

        return true;
#else
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
#if defined(MENGINE_PLATFORM_OSX)
        char * homeBuffer = getenv( "HOME" );

        if( homeBuffer == nullptr )
        {
            LOGGER_ERROR( "invalid get env 'HOME'" );

            return false;
        }

        Char path_music[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_music, MENGINE_MAX_PATH, "%s/Music/", homeBuffer );

        if( this->createDirectory( path_music, _directoryPath ) == false )
        {
            LOGGER_ERROR( "invalid create directory '%s%s'"
                , path_music
                , _directoryPath
            );

            return false;
        };

        Char path_file[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( path_file, MENGINE_MAX_PATH, "%s%s%s", path_music, _directoryPath, _filePath );

        SDL_RWops * rwops = SDL_RWFromFile( path_file, "wb" );

        if( rwops == nullptr )
        {
            LOGGER_ERROR( "invalid create file '%s'"
                , path_file
            );

            return false;
        }

        size_t written = SDL_RWwrite( rwops, _data, 1, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size %zu [error: %s]"
                , path_file
                , _size
                , SDL_GetError()
            );

            return false;
        }

        SDL_RWclose( rwops );

        return true;
#else
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::sleep( uint32_t _ms )
    {
        MENGINE_UNUSED( _ms );

        SDL_Delay( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, "invalid message box format message", nullptr );

            return;
        }

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, str, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::setClipboardText( const Char * _value ) const
    {
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getClipboardText( Char * _value, size_t _capacity ) const
    {
        MENGINE_UNUSED( _value );
        MENGINE_UNUSED( _capacity );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_Window * SDLPlatform::getWindow() const
    {
        return m_sdlWindow;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
#   if defined(MENGINE_WINDOWS_UNIVERSAL)
    //////////////////////////////////////////////////////////////////////////
    IInspectable * SDLPlatform::getWindowHandle() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        IInspectable * iwindow = wmInfo.info.winrt.window;

        return iwindow;
    }
    //////////////////////////////////////////////////////////////////////////
#   else
    //////////////////////////////////////////////////////////////////////////
    HWND SDLPlatform::getWindowHWND() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        HWND hwnd = wmInfo.info.win.window;

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    HDC SDLPlatform::getWindowHDC() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        HDC hdc = wmInfo.info.win.hdc;

        return hdc;
    }
    //////////////////////////////////////////////////////////////////////////
    HINSTANCE SDLPlatform::getWindowHinstance() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        HINSTANCE hinstance = wmInfo.info.win.hinstance;

        return hinstance;
    }
    //////////////////////////////////////////////////////////////////////////
#   endif
#elif defined(MENGINE_PLATFORM_LINUX)
    //////////////////////////////////////////////////////////////////////////
    Window SDLPlatform::getWindowInstance() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        Window w = wmInfo.info.x11.window;

        return w;
    }
    //////////////////////////////////////////////////////////////////////////
    Display * SDLPlatform::getWindowDisplay() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        Display * dispaly = wmInfo.info.x11.display;

        return dispaly;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
    JNIEnv * SDLPlatform::getJENV() const
    {
        return m_jenv;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass SDLPlatform::getJClassActivity() const
    {
        return jclass_activity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject SDLPlatform::getJObjectActivity() const
    {
        return jobject_activity;
    }
    //////////////////////////////////////////////////////////////////////////
    String SDLPlatform::getAndroidId() const
    {
        static jmethodID jmethodID_getAndroidId = m_jenv->GetMethodID( jclass_activity, "getAndroidId", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( jobject_activity, jmethodID_getAndroidId );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );

        String stringValue = jStringValue;

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        return stringValue;
    }
    //////////////////////////////////////////////////////////////////////////
    String SDLPlatform::getAndroidPackageName() const
    {
        static jmethodID jmethodID_getPackageName = m_jenv->GetMethodID( jclass_activity, "getPackageName", "()Ljava/lang/String;" );

        jstring jReturnValue = (jstring)m_jenv->CallObjectMethod( jobject_activity, jmethodID_getPackageName );

        const Char * jStringValue = m_jenv->GetStringUTFChars( jReturnValue, nullptr );

        String stringValue = jStringValue;

        m_jenv->ReleaseStringUTFChars( jReturnValue, jStringValue );

        return stringValue;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext SDLPlatform::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    uint32_t SDLPlatform::addSDLEventHandler( const LambdaSDLEventHandler & _handler )
    {
        UniqueId id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        SDLEventHandlerDesc desc;
        desc.id = id;
        desc.handler = _handler;

        m_sdlEventHandlers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::removeSDLEventHandler( uint32_t _handlerId )
    {
        VectorSDLEventHandlers::iterator it_found = Algorithm::find_if( m_sdlEventHandlers.begin(), m_sdlEventHandlers.end(), [_handlerId]( const SDLEventHandlerDesc & _desc )
        {
            return _desc.id == _handlerId;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_sdlEventHandlers.end() );

        m_sdlEventHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

#if defined(MENGINE_PLATFORM_IOS)
        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }
#elif defined(MENGINE_PLATFORM_ANDROID)
        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }
#elif defined(MENGINE_WINDOWS_UNIVERSAL)
        SDL_bool sdl_fullscreen = _fullscreen == true ? SDL_TRUE : SDL_FALSE;
        SDL_SetWindowFullscreen( m_sdlWindow, sdl_fullscreen );

        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }
#else
        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        RENDER_SYSTEM()
            ->onWindowChangeFullscreenPrepare( _fullscreen );

        this->destroyWindow_();
        this->createWindow( _resolution, _fullscreen );

        RENDER_SERVICE()
            ->onDeviceLostRestore();

        SDL_RaiseWindow( m_sdlWindow );
        SDL_ShowWindow( m_sdlWindow );

        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setupWindow_()
    {

#if defined(MENGINE_WINDOWS_UNIVERSAL)
#else
        uint32_t Engine_SDL_GL_RED_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_RED_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_RED_SIZE, Engine_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_RED_SIZE to [%u] error: %s"
                , Engine_SDL_GL_RED_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_GREEN_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_GREEN_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, Engine_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_GREEN_SIZE to [%u] error: %s"
                , Engine_SDL_GL_GREEN_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_BLUE_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_BLUE_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, Engine_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_BLUE_SIZE to [%u] error: %s"
                , Engine_SDL_GL_BLUE_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_ALPHA_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_ALPHA_SIZE", 0 );

        if( SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, Engine_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_ALPHA_SIZE to [%u] error: %s"
                , Engine_SDL_GL_ALPHA_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DEPTH_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_DEPTH_SIZE", 24 );

        if( SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, Engine_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_DEPTH_SIZE to [%u] error: %s"
                , Engine_SDL_GL_DEPTH_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DOUBLEBUFFER = CONFIG_VALUE( "SDL", "SDL_GL_DOUBLEBUFFER", 1 );

        if( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Engine_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_DOUBLEBUFFER to [%u] error: %s"
                , Engine_SDL_GL_DOUBLEBUFFER
                , SDL_GetError()
            );
        }
#endif

#if defined(MENGINE_PLATFORM_IOS)
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengles2" );
	
        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_ES );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_MAJOR_VERSION", 2 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE( "SDL", "Engine_SDL_GL_CONTEXT_MINOR_VERSION", 0 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_ORIENTATIONS = CONFIG_VALUE( "SDL", "SDL_HINT_ORIENTATIONS", "Portrait" );

        if( SDL_SetHint( SDL_HINT_ORIENTATIONS, Engine_SDL_HINT_ORIENTATIONS ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_ORIENTATIONS to [%s] error: %s"
                , Engine_SDL_HINT_ORIENTATIONS
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR = CONFIG_VALUE( "SDL", "SDL_HINT_IOS_HIDE_HOME_INDICATOR", "1" );

        if( SDL_SetHint( SDL_HINT_IOS_HIDE_HOME_INDICATOR, Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_IOS_HIDE_HOME_INDICATOR to [%s] error: %s"
                , Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR
                , SDL_GetError()
            );
        }

#elif defined(MENGINE_PLATFORM_ANDROID)
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengles2" );

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_ES );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_MAJOR_VERSION", 2 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE( "SDL", "Engine_SDL_GL_CONTEXT_MINOR_VERSION", 0 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_ORIENTATIONS = CONFIG_VALUE( "SDL", "SDL_HINT_ORIENTATIONS", "Portrait" );

        if( SDL_SetHint( SDL_HINT_ORIENTATIONS, Engine_SDL_HINT_ORIENTATIONS ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_ORIENTATIONS to [%s] error: %s"
                , Engine_SDL_HINT_ORIENTATIONS
                , SDL_GetError()
            );
        }

#elif defined(MENGINE_WINDOWS_UNIVERSAL)
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "direct3d11" );

#else
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengl" );

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_CORE );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_MAJOR_VERSION", 3 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_MINOR_VERSION", 2 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_ERROR( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY ) != SDL_TRUE )
        {
            LOGGER_ERROR( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY
                , SDL_GetError()
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        Uint32 windowFlags = 0;

#if defined(MENGINE_PLATFORM_IOS)
        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_SHOWN;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;

#elif defined(MENGINE_PLATFORM_ANDROID)
        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_SHOWN;
        windowFlags |= SDL_WINDOW_RESIZABLE;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;

#elif defined(MENGINE_WINDOWS_UNIVERSAL)
        windowFlags |= SDL_WINDOW_SHOWN;

        if( _fullscreen == true )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

#else
        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_HIDDEN;

        if( _fullscreen == true )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
#endif

        LOGGER_MESSAGE_RELEASE( "num video displays: %d"
            , SDL_GetNumVideoDisplays()
        );

#if defined(MENGINE_PLATFORM_IOS)
        SDL_Window * window = SDL_CreateWindow( m_projectTitle.c_str()
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#elif defined(MENGINE_PLATFORM_ANDROID)
        SDL_Window * window = SDL_CreateWindow( m_projectTitle.c_str()
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#else
        SDL_DisplayMode mode;
        if( SDL_GetDesktopDisplayMode( 0, &mode ) != 0 )
        {
            LOGGER_ERROR( "get desktop display mode error: %s"
                , SDL_GetError()
            );

            return false;
        }

        SDL_Window * window;

        if( _fullscreen == false )
        {
            int width = static_cast<int>(_windowResolution.getWidth());
            int height = static_cast<int>(_windowResolution.getHeight());

            uint32_t window_x_mode = (mode.w > width) ? SDL_WINDOWPOS_CENTERED : 50;
            uint32_t window_y_mode = (mode.h > height) ? SDL_WINDOWPOS_CENTERED : 50;

            window = SDL_CreateWindow( m_projectTitle.c_str()
                , window_x_mode
                , window_y_mode
                , width
                , height
                , windowFlags
            );
        }
        else
        {
            window = SDL_CreateWindow( m_projectTitle.c_str()
                , SDL_WINDOWPOS_UNDEFINED
                , SDL_WINDOWPOS_UNDEFINED
                , mode.w
                , mode.h
                , windowFlags
            );
        }
#endif

        if( window == nullptr )
        {
            LOGGER_ERROR( "create window failed: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_sdlWindow = window;

        LOGGER_MESSAGE_RELEASE( "SDL_HINT_RENDER_DRIVER: %s", SDL_GetHint( SDL_HINT_RENDER_DRIVER ) );
        LOGGER_MESSAGE_RELEASE( "SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetHint( SDL_HINT_RENDER_SCALE_QUALITY ) );
        LOGGER_MESSAGE_RELEASE( "SDL_HINT_ORIENTATIONS: %s", SDL_GetHint( SDL_HINT_ORIENTATIONS ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::destroyWindow_()
    {
#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
        if( m_glContext != nullptr )
        {
            SDL_GL_DeleteContext( m_glContext );
            m_glContext = nullptr;
        }
#endif

        if( m_sdlWindow != nullptr )
        {
            SDL_DestroyWindow( m_sdlWindow );
            m_sdlWindow = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
#if MENGINE_LOGGER_DEBUG
        //////////////////////////////////////////////////////////////////////////
        static const Char * getPlatformEventMessage( Uint32 _eventId )
        {
            switch( _eventId )
            {
                MENGINE_MESSAGE_CASE( SDL_QUIT, "User - requested quit" );
                MENGINE_MESSAGE_CASE( SDL_APP_TERMINATING, "The application is being terminated by the OS Called on iOS in applicationWillTerminate Called on Android in onDestroy" );
                MENGINE_MESSAGE_CASE( SDL_APP_LOWMEMORY, "The application is low on memory, free memory if possible.Called on iOS in applicationDidReceiveMemoryWarning Called on Android in onLowMemory" );
                MENGINE_MESSAGE_CASE( SDL_APP_WILLENTERBACKGROUND, "The application is about to enter the background Called on iOS in applicationWillResignActive Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_APP_DIDENTERBACKGROUND, "The application did enter the background and may not get CPU for some time Called on iOS in applicationDidEnterBackground Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_APP_WILLENTERFOREGROUND, "The application is about to enter the foreground Called on iOS in applicationWillEnterForeground Called on Android in onResume" );
                MENGINE_MESSAGE_CASE( SDL_APP_DIDENTERFOREGROUND, "The application is now interactive Called on iOS in applicationDidBecomeActive Called on Android in onResume" );
                MENGINE_MESSAGE_CASE( SDL_LOCALECHANGED, "The user's locale preferences have changed." );

                MENGINE_MESSAGE_CASE( SDL_DISPLAYEVENT, "Display state change" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT, "Window state change" );
                MENGINE_MESSAGE_CASE( SDL_SYSWMEVENT, "System specific event" );

                /* Keyboard events */
                MENGINE_MESSAGE_CASE( SDL_KEYDOWN, "Key pressed" );
                MENGINE_MESSAGE_CASE( SDL_KEYUP, "Key released" );
                MENGINE_MESSAGE_CASE( SDL_TEXTEDITING, "Keyboard text editing( composition )" );
                MENGINE_MESSAGE_CASE( SDL_TEXTINPUT, "Keyboard text input" );
                MENGINE_MESSAGE_CASE( SDL_KEYMAPCHANGED, "Keymap changed due to a system event such as an input language or keyboard layout change." );
                MENGINE_MESSAGE_CASE( SDL_MOUSEMOTION, "Mouse moved" );
                MENGINE_MESSAGE_CASE( SDL_MOUSEBUTTONDOWN, "Mouse button pressed" );
                MENGINE_MESSAGE_CASE( SDL_MOUSEBUTTONUP, "Mouse button released" );
                MENGINE_MESSAGE_CASE( SDL_MOUSEWHEEL, "Mouse wheel motion" );

                /* Joystick events */
                MENGINE_MESSAGE_CASE( SDL_JOYAXISMOTION, "Joystick axis motion" );
                MENGINE_MESSAGE_CASE( SDL_JOYBALLMOTION, "Joystick trackball motion" );
                MENGINE_MESSAGE_CASE( SDL_JOYHATMOTION, "Joystick hat position change" );
                MENGINE_MESSAGE_CASE( SDL_JOYBUTTONDOWN, "Joystick button pressed" );
                MENGINE_MESSAGE_CASE( SDL_JOYBUTTONUP, "Joystick button released" );
                MENGINE_MESSAGE_CASE( SDL_JOYDEVICEADDED, "A new joystick has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_JOYDEVICEREMOVED, "An opened joystick has been removed" );

                /* Game controller events */
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERAXISMOTION, "Game controller axis motion" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERBUTTONDOWN, "Game controller button pressed" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERBUTTONUP, "Game controller button released" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERDEVICEADDED, "A new Game controller has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERDEVICEREMOVED, "An opened Game controller has been removed" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERDEVICEREMAPPED, "The controller mapping was updated" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERTOUCHPADDOWN, "Game controller touchpad was touched" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERTOUCHPADMOTION, "Game controller touchpad finger was moved" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERTOUCHPADUP, "Game controller touchpad finger was lifted" );
                MENGINE_MESSAGE_CASE( SDL_CONTROLLERSENSORUPDATE, "Game controller sensor was updated" );

                /* Touch events */
                MENGINE_MESSAGE_CASE( SDL_FINGERDOWN, "SDL_FINGERDOWN" );
                MENGINE_MESSAGE_CASE( SDL_FINGERUP, "SDL_FINGERUP" );
                MENGINE_MESSAGE_CASE( SDL_FINGERMOTION, "SDL_FINGERMOTION" );

                /* Gesture events */
                MENGINE_MESSAGE_CASE( SDL_DOLLARGESTURE, "SDL_DOLLARGESTURE" );
                MENGINE_MESSAGE_CASE( SDL_DOLLARRECORD, "SDL_DOLLARRECORD" );
                MENGINE_MESSAGE_CASE( SDL_MULTIGESTURE, "SDL_MULTIGESTURE" );

                /* Clipboard events */
                MENGINE_MESSAGE_CASE( SDL_CLIPBOARDUPDATE, "The clipboard changed" );

                /* Drag and drop events */
                MENGINE_MESSAGE_CASE( SDL_DROPFILE, "The system requests a file open" );
                MENGINE_MESSAGE_CASE( SDL_DROPTEXT, "text/plain drag-and-drop event" );
                MENGINE_MESSAGE_CASE( SDL_DROPBEGIN, "A new set of drops is beginning (NULL filename)" );
                MENGINE_MESSAGE_CASE( SDL_DROPCOMPLETE, "Current set of drops is now complete (NULL filename)" );

                /* Audio hotplug events */
                MENGINE_MESSAGE_CASE( SDL_AUDIODEVICEADDED, "A new audio device is available" );
                MENGINE_MESSAGE_CASE( SDL_AUDIODEVICEREMOVED, "An audio device has been removed" );

                /* Sensor events */
                MENGINE_MESSAGE_CASE( SDL_SENSORUPDATE, "A sensor was updated" );

                /* Render events */
                MENGINE_MESSAGE_CASE( SDL_RENDER_TARGETS_RESET, "The render targets have been reset and their contents need to be updated" );
                MENGINE_MESSAGE_CASE( SDL_RENDER_DEVICE_RESET, "The device has been reset and all textures need to be recreated" );

                /** Events ::SDL_USEREVENT through ::SDL_LASTEVENT are for your use,
                 *  and should be allocated with SDL_RegisterEvents()
                 */
                MENGINE_MESSAGE_CASE( SDL_USEREVENT, "SDL_USEREVENT" );
            default:
                break;
            }

            return "UNKNOWN";
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * getWindowEventMessage( SDL_WindowEventID _eventId )
        {
            switch( _eventId )
            {
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_NONE, "Never used" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_SHOWN, "Window has been shown" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_HIDDEN, "Window has been hidden" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_EXPOSED, "Window has been exposed and should be redrawn" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_MOVED, "Window has been moved to data1, data2" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_RESIZED, "Window has been resized to data1xdata2" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_SIZE_CHANGED, "The window size has changed, either as a result of an API call or through the system or user changing the window size." );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_MINIMIZED, "Window has been minimized" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_MAXIMIZED, "Window has been maximized" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_RESTORED, "Window has been restored to normal size and position" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_ENTER, "Window has gained mouse focus" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_LEAVE, "Window has lost mouse focus" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_FOCUS_GAINED, "Window has gained keyboard focus" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_FOCUS_LOST, "Window has lost keyboard focus" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_CLOSE, "The window manager requests that the window be closed" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_TAKE_FOCUS, "Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore)" );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_HIT_TEST, "Window had a hit test that wasn't SDL_HITTEST_NORMAL." );
            default:
                {
                }break;
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::processEvents_()
    {
        SDL_PumpEvents();

        SDL_Event sdlEvent;
        while( SDL_PollEvent( &sdlEvent ) != 0 )
        {
            LOGGER_INFO( "platform", "platform event: %s (%u)"
                , Helper::getPlatformEventMessage( sdlEvent.type )
                , sdlEvent.type
            );

            for( const SDLEventHandlerDesc & desc : m_sdlEventHandlers )
            {
                desc.handler( &sdlEvent );
            }

            m_sdlInput->handleEvent( m_sdlWindow, sdlEvent );

            switch( sdlEvent.type )
            {
            case SDL_KEYDOWN:
                {
                    switch( sdlEvent.key.keysym.sym )
                    {
                    case SDLK_RETURN:
                        {
                            if( sdlEvent.key.keysym.mod & KMOD_ALT )
                            {
                                bool fullscreen = APPLICATION_SERVICE()
                                    ->getFullscreenMode();

                                APPLICATION_SERVICE()
                                    ->setFullscreenMode( !fullscreen );
                            }
                        }break;
                    default:
                        break;
                    }
                }break;
            case SDL_WINDOWEVENT:
                {
                    Uint32 windowID = SDL_GetWindowID( m_sdlWindow );

                    if( sdlEvent.window.windowID != windowID )
                    {
                        continue;
                    }

                    SDL_WindowEventID windowEventId = (SDL_WindowEventID)sdlEvent.window.event;

                    LOGGER_INFO( "platform", "window event: %s (%u)"
                        , Helper::getWindowEventMessage( windowEventId )
                        , windowEventId
                    );

                    switch( windowEventId )
                    {
                    case SDL_WINDOWEVENT_SHOWN:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_MOVED:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_RESIZED:
                        {
                            Resolution windowResolution( sdlEvent.window.data1, sdlEvent.window.data2 );

                            APPLICATION_SERVICE()
                                ->changeWindowResolution( windowResolution );
                        }break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                    case SDL_WINDOWEVENT_ENTER:
                        {
                            this->setActive_( true );
                        }break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                    case SDL_WINDOWEVENT_MINIMIZED:
                    case SDL_WINDOWEVENT_LEAVE:
                        {
                            this->setActive_( false );
                        }break;
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                            SDL_Event newEvent;
                            newEvent = sdlEvent;
                            newEvent.type = SDL_QUIT;

                            if( SDL_PushEvent( &newEvent ) == -1 )
                            {
                                LOGGER_ERROR( "invalid push event [%u] error: %s"
                                    , newEvent.type
                                    , SDL_GetError()
                                );
                            }
                        }break;
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
                        {
                            //TODO
                        }break;
                    default:
                        break;
                    }
                }break;
            case SDL_QUIT:
                {
                    m_shouldQuit = true;
                }break;
            default:
                break;
            }
        }

        if( m_shouldQuit == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setActive_( bool _active )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        mt::vec2f point;
        m_sdlInput->getCursorPosition( m_sdlWindow, &point );

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( TC_TOUCH0, point.x, point.y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( TC_TOUCH0, point.x, point.y, 0.f );
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
    bool SDLPlatform::isNeedWindowRender() const
    {
        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        if( nopause == true )
        {
            return true;
        }

        if( m_active == false )
        {
            return false;
        }

        bool focus = APPLICATION_SERVICE()
            ->isFocus();

        if( focus == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::initializeFileService_()
    {
        LOGGER_INFO( "platform", "Initialize SDL file group..." );

        PLUGIN_CREATE( SDLFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory" );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        // mount root
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), defaultFileGroup, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory" );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::finalizeFileService_()
    {
#ifndef MENGINE_MASTER_RELEASE
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyInitializeBaseServices_()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        static jmethodID jmethodID_onMengineInitializeBaseServices = m_jenv->GetMethodID( jclass_activity, "onMengineInitializeBaseServices", "()V" );

        if( jmethodID_onMengineInitializeBaseServices != NULL )
        {
            m_jenv->CallVoidMethod( jobject_activity, jmethodID_onMengineInitializeBaseServices );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyCreateApplication_()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        static jmethodID jmethodID_onMengineCreateApplication = m_jenv->GetMethodID( jclass_activity, "onMengineCreateApplication", "()V" );

        if( jmethodID_onMengineCreateApplication != NULL )
        {
            m_jenv->CallVoidMethod( jobject_activity, jmethodID_onMengineCreateApplication );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatform::androidOpenAssetFile( const Char * _path )
    {
        int32_t fileId = AndroidOpenAssetFile( m_jenv, jclass_activity, jobject_activity, _path );

        return fileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatform::androidAvailableAssetFile( int32_t _fileId )
    {
        int32_t available = AndroidAvailableAssetFile( m_jenv, jclass_activity, jobject_activity, _fileId );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatform::androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        int32_t read = AndroidReadAssetFile( m_jenv, jclass_activity, jobject_activity, _fileId, _offset, _size, _buffer );

        return read;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDLPlatform::androidSkipAssetFile( int32_t _fileId, int32_t _offset )
    {
        int32_t skip = AndroidSkipAssetFile( m_jenv, jclass_activity, jobject_activity, _fileId, _offset );

        return skip;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::androidResetAssetFile( int32_t _fileId )
    {
        AndroidResetAssetFile( m_jenv, jclass_activity, jobject_activity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::androidCloseAssetFile( int32_t _fileId )
    {
        AndroidCloseAssetFile( m_jenv, jclass_activity, jobject_activity, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
