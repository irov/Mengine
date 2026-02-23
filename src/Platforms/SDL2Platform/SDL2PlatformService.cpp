#include "SDL2PlatformService.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#elif defined(MENGINE_VENDOR_APPLE)
#   if defined(MENGINE_PLATFORM_MACOS)
#       include "Environment/MacOS/MacOSUtils.h"
#   endif
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32Helper.h"
#   include "Environment/Windows/Win32FileHelper.h"
#endif

#include "SDL2DynamicLibrary.h"

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

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/Switch.h"
#include "Config/Path.h"

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
SERVICE_FACTORY( PlatformService, Mengine::SDL2PlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2PlatformService::SDL2PlatformService()
        : m_beginTime( 0 )
        , m_sdlWindow( nullptr )
        , m_sdlAccelerometer( nullptr )
        , m_sdlInput( nullptr )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_desktop( false )
        , m_touchpad( false )
#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
        , m_glContext( nullptr )
#endif
#if defined( MENGINE_PLATFORM_MACOS )
        , m_macOSWorkspace( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2PlatformService::~SDL2PlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t SDL2PlatformService::getCurrentPath( Char * const _currentPath ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        WPath unicode_currentPath = {L'\0'};
        DWORD unicode_currentPathLen = ::GetCurrentDirectory( MENGINE_MAX_PATH - 2, unicode_currentPath );

        if( unicode_currentPathLen == 0 )
        {
            StdString::strcpy_safe( _currentPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        ::PathRemoveBackslashW( unicode_currentPath );
        ::PathAddBackslashW( unicode_currentPath );

        Helper::pathCorrectBackslashW( unicode_currentPath );

        size_t pathLen;
        if( Helper::unicodeToUtf8( unicode_currentPath, _currentPath, MENGINE_MAX_PATH, &pathLen ) == false )
        {
            StdString::strcpy_safe( _currentPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        return pathLen;
#elif defined(MENGINE_PLATFORM_MACOS)
        char * basePath = SDL_GetBasePath();

        StdString::strcpy_safe( _currentPath, basePath, MENGINE_MAX_PATH );

        SDL_free( basePath );

        return StdString::strlen( _currentPath );
#else
        StdString::strcpy_safe( _currentPath, "", MENGINE_MAX_PATH );

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2PlatformService::getUserPath( Char * const _userPath ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roaming = HAS_OPTION( "roaming" );
        bool OPTION_noroaming = HAS_OPTION( "noroaming" );

        if( (developmentMode == true && OPTION_roaming == false) || OPTION_noroaming == true )
        {
            Path currentPath = {'\0'};
            size_t currentPathLen = this->getCurrentPath( currentPath );

            if( MENGINE_MAX_PATH <= currentPathLen + 5 )
            {
                StdString::strcpy_safe( _userPath, "", MENGINE_MAX_PATH );

                return 0;
            }

            StdString::strcpy_safe( _userPath, currentPath, MENGINE_MAX_PATH );
            StdString::strcat_safe( _userPath, MENGINE_DEVELOPMENT_USER_FOLDER_NAME, MENGINE_MAX_PATH );
            StdString::strchrcat_safe( _userPath, MENGINE_PATH_FORWARDSLASH, MENGINE_MAX_PATH );

            size_t pathLen = StdString::strlen( _userPath );

            return pathLen;
        }
#endif

        PathString Project_Company = CONFIG_VALUE_PATHSTRING( "Project", "Company", "UNKNOWN" );
        PathString Project_Name = CONFIG_VALUE_PATHSTRING( "Project", "Name", "UNKNOWN" );

        Char * sdl_prefPath = SDL_GetPrefPath( Project_Company.c_str(), Project_Name.c_str() );

        size_t sdl_prefPathLen = StdString::strlen( _userPath );

        if( sdl_prefPathLen >= MENGINE_MAX_PATH )
        {
            SDL_free( sdl_prefPath );

            StdString::strcpy_safe( _userPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        Helper::pathCorrectBackslashToA( _userPath, sdl_prefPath );

        SDL_free( sdl_prefPath );

        Path extraPreferencesFolderName = {'\0'};
        size_t ExtraPreferencesFolderNameLen = this->getExtraPreferencesFolderName( extraPreferencesFolderName );

        if( ExtraPreferencesFolderNameLen != 0 )
        {
            Helper::pathAppendA( _userPath, extraPreferencesFolderName, MENGINE_PATH_FORWARDSLASH );
        }

        size_t userPathLen = StdString::strlen( _userPath );

        return userPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2PlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        PathString Project_ExtraPreferencesFolderName = CONFIG_VALUE_PATHSTRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( Project_ExtraPreferencesFolderName.c_str() ) == true, "invalid extra preferences folder name '%s'"
            , Project_ExtraPreferencesFolderName.c_str()
        );

        StdString::strcpy_safe( _folderName, Project_ExtraPreferencesFolderName.c_str(), MENGINE_MAX_PATH );
        
        size_t Project_ExtraPreferencesFolderNameLen = Project_ExtraPreferencesFolderName.size();

        return Project_ExtraPreferencesFolderNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        SDL_Locale * locale = SDL_GetPreferredLocales();
        
        if( locale == nullptr )
        {
            LOGGER_ERROR( "invalid get preferred locales: %s"
                , SDL_GetError()
            );

            return false;
        }

        _userLocaleLanguage[0] = (Char)locale->language[0];
        _userLocaleLanguage[1] = (Char)locale->language[1];
        _userLocaleLanguage[2] = '\0';

        SDL_free( locale );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL2PlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        int displayIndex = -1;
        
        if( m_sdlWindow == nullptr )
        {
            displayIndex = 0;
        }
        else
        {
            displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );

            if( displayIndex == -1 )
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

        SDL_Rect rect;
        if( SDL_GetDisplayUsableBounds( displayIndex, &rect ) != 0 )
        {
            LOGGER_ERROR( "invalid get max client resolution: %s"
                , SDL_GetError()
            );

            return false;
        }

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
        static const Char * SDL_GetLoggerCategoryString( int category )
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
                return LM_DEBUG;
            case SDL_LOG_PRIORITY_INFO:
                return LM_INFO;
            case SDL_LOG_PRIORITY_WARN:
                return LM_WARNING;
            case SDL_LOG_PRIORITY_ERROR:
                return LM_ERROR;
            case SDL_LOG_PRIORITY_CRITICAL:
                return LM_FATAL;
            default:
                break;
            }

            return LM_ERROR;
        }
        //////////////////////////////////////////////////////////////////////////
        static void SDL_LogOutputFunction( void * userdata, int category, SDL_LogPriority priority, const char * message )
        {
            MENGINE_UNUSED( userdata );

            ELoggerLevel level = Detail::SDL_GetLoggerLevel( priority );
            const Char * category_str = Detail::SDL_GetLoggerCategoryString( category );

            LOGGER_VERBOSE_LEVEL( "sdl", level, LFILTER_NONE, LCOLOR_RED, nullptr, 0, nullptr, LFLAG_SHORT )("[%s] %s"
                , category_str
                , message
            );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        SDL_version ver;
        SDL_GetVersion( &ver );

        const char * revision = SDL_GetRevision();

        LOGGER_INFO( "platform", "SDL2 version: %u.%u.%u revision: %s"
            , ver.major
            , ver.minor
            , ver.patch
            , revision
        );

        SDL_bool isTablet = SDL_IsTablet();

        LOGGER_INFO( "platform", "SDL2 Tablet: %s"
            , isTablet == SDL_TRUE ? "true" : "false"
        );

#if defined(MENGINE_DEBUG)
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_VERBOSE );
#else
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_ERROR );
#endif

        SDL_LogSetOutputFunction( &Detail::SDL_LogOutputFunction, nullptr );

        m_platformTags.clear();

        const Char * option_platforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t option_platforms_count;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "platform", option_platforms, &option_platforms_count ) == true )
        {
            for( uint32_t index = 0; index != option_platforms_count; ++index )
            {
                const Char * option_platform = option_platforms[index];

                Char uppercase_option_platform[256 + 1] = {'\0'};;
                Helper::toupper( option_platform, uppercase_option_platform, 255 );

                m_platformTags.addTag( Helper::stringizeString( option_platform ) );
            }
        }
        else if( HAS_OPTION( "win32" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
        }
        else if( HAS_OPTION( "win64" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
        }
        else if( HAS_OPTION( "mac" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );
        }
        else if( HAS_OPTION( "ios" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );
        }
        else if( HAS_OPTION( "android" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );
        }
        else if( HAS_OPTION( "linux" ) )
        {
            m_touchpad = false;
            m_desktop = true;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "LINUX" ) );
        }
        else if( HAS_OPTION( "wp" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WP" ) );
        }
        else
        {
            const Char * sdlPlatform = SDL_GetPlatform();

            if( StdString::strcmp( sdlPlatform, "Windows" ) == 0 )
            {
                m_desktop = true;
                m_touchpad = false;

                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );

#if defined(MENGINE_PLATFORM_WINDOWS32)
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS64)
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN64" ) );
#endif
            }
            else if( StdString::strcmp( sdlPlatform, "WinRT" ) == 0 )
            {
                m_desktop = true;
                m_touchpad = false;

                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WinRT" ) );

                SDL_SetEventFilter( &Detail::SDL_EventFilter_EnterBackground, nullptr );
            }
            else if( StdString::strcmp( sdlPlatform, "Mac OS X" ) == 0 )
            {
                m_desktop = true;
                m_touchpad = false;

                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MAC" ) );
            }
            else if( StdString::strcmp( sdlPlatform, "Android" ) == 0 )
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
            else if( StdString::strcmp( sdlPlatform, "iOS" ) == 0 )
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
            else if( StdString::strcmp( sdlPlatform, "Linux" ) == 0 )
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
        }

        if( HAS_OPTION( "table" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "TABLET" ) );
        }
        else if( HAS_OPTION( "phone" ) == true )
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PHONE" ) );
        }

        Char platformTags[1024 + 1] = {'\0'};
        StdString::strcpy_safe( platformTags, "", 1024 );

        for( const ConstString & tag : m_platformTags.getValues() )
        {
            StdString::strcat_safe( platformTags, tag.c_str(), 1024 );
            StdString::strcat_safe( platformTags, "-", 1024 );
        }

        LOGGER_INFO( "platform", "platform tags: %s"
            , platformTags
        );

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        LOGGER_INFO( "platform", "touchpad: %u"
            , m_touchpad
        );

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        LOGGER_INFO( "platform", "desktop: %u"
            , m_desktop
        );

        SDL2InputPtr sdlInput = Helper::makeFactorableUnique<SDL2Input>( MENGINE_DOCUMENT_FACTORABLE );

        if( sdlInput->initialize() == false )
        {
            return false;
        }

        m_sdlInput = sdlInput;

        m_factoryDynamicLibraries = Helper::makeFactoryPool<SDL2DynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::_runService()
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

        LOGGER_INFO( "platform", "[Device info]" );

        LOGGER_INFO( "platform", "  Platform: %s"
            , SDL_GetPlatform()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU: %d Count %d CacheLineSize"
            , SDL_GetCPUCount()
            , SDL_GetCPUCacheLineSize()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU RDTSC: %d"
            , SDL_HasRDTSC()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU AltiVec: %d"
            , SDL_HasAltiVec()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU MMX: %d"
            , SDL_HasMMX()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU 3DNow: %d"
            , SDL_Has3DNow()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU SSE: %d"
            , SDL_HasSSE()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU SSE2: %d"
            , SDL_HasSSE2()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU SSE3: %d"
            , SDL_HasSSE3()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU SSE41: %d"
            , SDL_HasSSE41()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU SSE42: %d"
            , SDL_HasSSE42()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU AVX: %d"
            , SDL_HasAVX()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU AVX2: %d"
            , SDL_HasAVX2()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU AVX512F: %d"
            , SDL_HasAVX512F()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU NEON: %d"
            , SDL_HasNEON()
        );

        LOGGER_INFO_PROTECTED( "platform", "  RAM: %dMB"
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

            if( StdString::strcmp( joystickName, "Android Accelerometer" ) == 0 )
            {
                isAccelerometer = true;
            }
            else if( StdString::strcmp( joystickName, "iOS Accelerometer" ) == 0 )
            {
                isAccelerometer = true;
            }

            if( isAccelerometer == true )
            {
                LOGGER_INFO( "platform", "accelerometer found: %s"
                    , joystickName
                );

                m_sdlAccelerometer = joystick;

                break;
            }
        }

        if( m_sdlAccelerometer == nullptr )
        {
            LOGGER_INFO( "platform", "accelerometer not found" );
        }

#if defined(MENGINE_PLATFORM_MACOS)
        //cppcheck-suppress syntaxError
        m_macOSWorkspace = [[MacOSWorkspace alloc] initialize];        
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::_stopService()
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
    void SDL2PlatformService::_finalizeService()
    {
        m_active = false;

        this->destroyWindow_();

        if( m_sdlInput != nullptr )
        {
            m_sdlInput->finalize();
            m_sdlInput = nullptr;
        }
        
#if defined(MENGINE_PLATFORM_MACOS)
        m_macOSWorkspace = nil;
#endif

        m_platformTags.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::runPlatform()
    {
        this->setActive_( true );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::tickPlatform( float _frameTime )
    {
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

        APPLICATION_SERVICE()
            ->endUpdate();

#if defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_MACOS)
        if( updating == false )
        {
            if( m_pauseUpdatingTime < 0.f )
            {
                m_pauseUpdatingTime = _frameTime;
            }

            SDL_Delay( 1 );
        }
        else
        {
            bool OPTION_maxfps = HAS_OPTION( "maxfps" );

            if( APPLICATION_SERVICE()
                ->getVSync() == false && OPTION_maxfps == false )
            {
                SDL_Delay( 1 );
            }
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::renderPlatform()
    {
        if( this->isNeedWindowRender() == false )
        {
            return false;
        }

        bool sucessful = APPLICATION_SERVICE()
            ->render();

        if( sucessful == false )
        {
            return false;
        }

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::loopPlatform()
    {
        m_prevTime = Helper::getElapsedTime();

        for( ;; )
        {
            if( this->updatePlatform() == false )
            {
                break;
            }

            double currentTime = Helper::getElapsedTime();
            
            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            if( m_active == false )
            {
                SDL_Delay( 100 );

                continue;
            }

            this->tickPlatform( frameTime );

            if( this->renderPlatform() == false )
            {
                SDL_Delay( 100 );

                continue;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_ERROR( "not supported open url in default browser '%s'"
            , _url
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        LOGGER_ERROR( "not supported open mail '%s' subject '%s' body '%s'"
            , _email
            , _subject
            , _body
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::openDeleteAccount()
    {
        LOGGER_INFO( "platform", "open delete account" );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp SDL2PlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::setProjectTitle( const Char * _projectTitle )
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
    size_t SDL2PlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
    {
        LOGGER_INFO( "platform", "create window size %u:%u fullscreen %d"
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
    bool SDL2PlatformService::applyWindow_()
    {
#if defined(MENGINE_PLATFORM_UWP)
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
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_PROFILE_MASK error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MAJOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &attribute_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MAJOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MINOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &attribute_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MINOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_RED_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &attribute_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_RED_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_GREEN_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &attribute_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_GREEN_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_BLUE_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &attribute_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_BLUE_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_ALPHA_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &attribute_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_ALPHA_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DEPTH_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &attribute_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DOUBLEBUFFER = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &attribute_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_DOUBLEBUFFER error: %s"
                , SDL_GetError()
            );
        }

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_PROFILE_MASK: %d"
            , attribute_GL_CONTEXT_PROFILE_MASK
        );

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_MAJOR_VERSION: %d"
            , attribute_GL_CONTEXT_MAJOR_VERSION
        );

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_MINOR_VERSION: %d"
            , attribute_GL_CONTEXT_MINOR_VERSION
        );

        LOGGER_INFO( "platform", "SDL_GL_RED_SIZE: %d"
            , attribute_SDL_GL_RED_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_GREEN_SIZE: %d"
            , attribute_SDL_GL_GREEN_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_BLUE_SIZE: %d"
            , attribute_SDL_GL_BLUE_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_ALPHA_SIZE: %d"
            , attribute_SDL_GL_ALPHA_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_DEPTH_SIZE: %d"
            , attribute_SDL_GL_DEPTH_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_DOUBLEBUFFER: %d"
            , attribute_SDL_GL_DOUBLEBUFFER
        );

        m_glContext = glContext;

        int drawable_width;
        int drawable_height;
        SDL_GL_GetDrawableSize( m_sdlWindow, &drawable_width, &drawable_height );

        LOGGER_INFO( "platform", "SDL2 drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );
#endif

        int win_width;
        int win_height;
        SDL_GetWindowSize( m_sdlWindow, &win_width, &win_height );

        LOGGER_INFO( "platform", "SDL2 window size [%d, %d]"
            , win_width
            , win_height
        );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        int win_top;
        int win_left;
        int win_bottom;
        int win_right;
        if( SDL_GetWindowBordersSize( m_sdlWindow, &win_top, &win_left, &win_bottom, &win_right ) != 0 )
        {
            LOGGER_WARNING( "SDL2 window borders get error: %s"
                , SDL_GetError()
            );
        }

        LOGGER_INFO( "platform", "SDL2 window borders [%d, %d] - [%d, %d]"
            , win_left
            , win_top
            , win_right
            , win_bottom
        );
#endif

        int win_min_width;
        int win_min_height;
        SDL_GetWindowMinimumSize( m_sdlWindow, &win_min_width, &win_min_height );

        LOGGER_INFO( "platform", "SDL2 window min size [%d, %d]"
            , win_min_width
            , win_min_height
        );

        int win_max_width;
        int win_max_height;
        SDL_GetWindowMaximumSize( m_sdlWindow, &win_max_width, &win_max_height );

        LOGGER_INFO( "platform", "SDL2 window max size [%d, %d]"
            , win_max_width
            , win_max_height
        );

        Uint32 flags = SDL_GetWindowFlags( m_sdlWindow );

        if( (flags & SDL_WINDOW_FULLSCREEN) != SDL_WINDOW_FULLSCREEN )
        {
            int displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );

            if( displayIndex >= 0 )
            {
                SDL_Rect displayBounds;
                if( SDL_GetDisplayBounds( displayIndex, &displayBounds ) == 0 )
                {
                    LOGGER_INFO( "platform", "SDL2 display bounds [%d, %d] size [%d, %d]"
                        , displayBounds.x
                        , displayBounds.y
                        , displayBounds.w
                        , displayBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL2 display [%d] bounds get error: %s"
                        , displayIndex
                        , SDL_GetError()
                    );
                }

                SDL_Rect usableBounds;
                if( SDL_GetDisplayUsableBounds( displayIndex, &usableBounds ) == 0 )
                {
                    LOGGER_INFO( "platform", "SDL2 display usable bounds [%d, %d] size [%d, %d]"
                        , usableBounds.x
                        , usableBounds.y
                        , usableBounds.w
                        , usableBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL2 display [%d] usable bounds get error: %s"
                        , displayIndex
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::attachWindow( const void * _hWND )
    {
        this->setupWindow_();

#if defined(MENGINE_PLATFORM_WINDOWS)
        SDL_Window * sharePixelFormatWindow = SDL_CreateWindow( "MengineSharePixelFormatWindow", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );

        Char sBuf[64 + 1] = {'\0'};
        MENGINE_SNPRINTF( sBuf, 64, "%p", sharePixelFormatWindow );

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
#else
        MENGINE_UNUSED( _hWND );

        LOGGER_ERROR( "SDL3PlatformService::attachWindow unsupported for this platform" );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & SDL2PlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL2PlatformService::dpToWidthPx( int32_t _dp ) const
    {
        MENGINE_UNUSED( _dp );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL2PlatformService::dpToHeightPx( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL2PlatformService::pxToWidthDp( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL2PlatformService::pxToHeightDp( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr SDL2PlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        SDL2DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

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
    bool SDL2PlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        uint32_t width;
        uint32_t height;

        int displayIndex;

        if( m_sdlWindow == nullptr )
        {
            displayIndex = 0;
        }
        else
        {
            displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );

            if( displayIndex == -1 )
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

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

        * _resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::isDebuggerPresent() const
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
    void SDL2PlatformService::debugBreak()
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        ::DebugBreak();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::closeWindow()
    {
        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::minimizeWindow()
    {
        SDL_MinimizeWindow( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        Resolution resolution;
        if( this->getDesktopResolution( &resolution ) == false )
        {
            return;
        }

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        int32_t wndPosX = static_cast<int32_t>(_pos.x * width);
        int32_t wndPosY = static_cast<int32_t>(_pos.y * height);

        // ! This function generates a mouse motion event !
        SDL_WarpMouseInWindow( m_sdlWindow, wndPosX, wndPosY );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::showKeyboard()
    {
        SDL_StartTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::hideKeyboard()
    {
        SDL_StopTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::isShowKeyboard() const
    {
        SDL_bool active = SDL_IsTextInputActive();

        return active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_sdlWindow == nullptr )
        {
            return true;
        }

        Uint32 flags = SDL_GetWindowFlags( m_sdlWindow );

        bool alreadyFullscreen = (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;

        if( _fullscreen != alreadyFullscreen )
        {
            if( this->changeWindow_( _resolution, _fullscreen ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::notifyVsyncChanged( bool _vsync )
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
                    LOGGER_WARNING( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                if( SDL_GL_SetSwapInterval( 1 ) != 0 )
                {
                    LOGGER_WARNING( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::notifyCursorModeChanged( bool _mode )
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
    bool SDL2PlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

#if defined(MENGINE_PLATFORM_MACOS)
        Path path_pictures = {'\0'};
        if( Helper::MacOSGetPicturesDirectory( path_pictures ) == false )
        {
            LOGGER_ERROR( "invalid get pictures directory" );
            
            return false;
        }
        
        Path path_file = {'\0'};
        MENGINE_SNPRINTF( path_file, MENGINE_MAX_PATH, "%s%s%s", path_pictures, _directoryPath, _filePath );

        if( Helper::MacOSSetDesktopWallpaper( path_file ) == false )
        {
            LOGGER_ERROR( "error set desktop wallpaper '%s'"
                , path_file
            );
        }
        
        Uint32 flags = SDL_GetWindowFlags( m_sdlWindow );

        if( (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN )
        {
            [m_macOSWorkspace changeDesktopWallpaper:path_file];
        }
#endif

        //MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
#if defined(MENGINE_PLATFORM_MACOS)
        Path path_pictures = {'\0'};
        if( Helper::MacOSGetPicturesDirectory( path_pictures ) == false )
        {
            LOGGER_ERROR( "invalid get pictures directory" );
            
            return false;
        }

        if( this->createDirectory( path_pictures, _directoryPath ) == false )
        {
            LOGGER_ERROR( "invalid create directory '%s%s'"
                , path_pictures
                , _directoryPath
            );

            return false;
        };

        Path path_file = {'\0'};
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
    bool SDL2PlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
#if defined(MENGINE_PLATFORM_MACOS)
        Path path_music = {'\0'};
        if( Helper::MacOSGetMusicDirectory( path_music ) == false )
        {
            LOGGER_ERROR( "invalid get music directory" );
            
            return false;
        }

        if( this->createDirectory( path_music, _directoryPath ) == false )
        {
            LOGGER_ERROR( "invalid create directory '%s%s'"
                , path_music
                , _directoryPath
            );

            return false;
        };

        Path path_file = {'\0'};
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
    void SDL2PlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
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

            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, "invalid message box format message", nullptr );

            return;
        }

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, str, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::setClipboardText( const Char * _value ) const
    {
        if( SDL_SetClipboardText( _value ) != 0 )
        {
            LOGGER_WARNING( "set clipboard text [%s] error: %s"
                , _value
                , SDL_GetError()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        char * text = SDL_GetClipboardText();

        if( text == nullptr )
        {
            return false;
        }

        StdString::strcpy_safe( _value, text, _capacity );

        SDL_free( text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_Window * SDL2PlatformService::getWindow() const
    {
        return m_sdlWindow;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
#   if defined(MENGINE_PLATFORM_UWP)
    //////////////////////////////////////////////////////////////////////////
    IInspectable * SDL2PlatformService::getWindowHandle() const
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
    HWND SDL2PlatformService::getWindowHWND() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        HWND hwnd = wmInfo.info.win.window;

        return hwnd;
    }
    //////////////////////////////////////////////////////////////////////////
    HDC SDL2PlatformService::getWindowHDC() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        HDC hdc = wmInfo.info.win.hdc;

        return hdc;
    }
    //////////////////////////////////////////////////////////////////////////
    HINSTANCE SDL2PlatformService::getWindowHinstance() const
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
    Window SDL2PlatformService::getWindowInstance() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        Window w = wmInfo.info.x11.window;

        return w;
    }
    //////////////////////////////////////////////////////////////////////////
    Display * SDL2PlatformService::getWindowDisplay() const
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
#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext SDL2PlatformService::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    UniqueId SDL2PlatformService::addSDLEventHandler( const LambdaSDLEventHandler & _handler )
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
    void SDL2PlatformService::removeSDLEventHandler( UniqueId _handlerId )
    {
        VectorSDLEventHandlers::iterator it_found = StdAlgorithm::find_if( m_sdlEventHandlers.begin(), m_sdlEventHandlers.end(), [_handlerId]( const SDLEventHandlerDesc & _desc )
        {
            return _desc.id == _handlerId;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_sdlEventHandlers.end(), "not found handler '%u'"
            , _handlerId
        );

        m_sdlEventHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

#if defined(MENGINE_PLATFORM_UWP)
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

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            return false;
        }

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
    void SDL2PlatformService::setupWindow_()
    {
#if defined(MENGINE_PLATFORM_UWP)
#else
        uint32_t Engine_SDL_GL_RED_SIZE = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_RED_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_RED_SIZE, Engine_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_RED_SIZE to [%u] error: %s"
                , Engine_SDL_GL_RED_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_GREEN_SIZE = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_GREEN_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, Engine_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_GREEN_SIZE to [%u] error: %s"
                , Engine_SDL_GL_GREEN_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_BLUE_SIZE = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_BLUE_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, Engine_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_BLUE_SIZE to [%u] error: %s"
                , Engine_SDL_GL_BLUE_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_ALPHA_SIZE = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_ALPHA_SIZE", 0U );

        if( SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, Engine_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_ALPHA_SIZE to [%u] error: %s"
                , Engine_SDL_GL_ALPHA_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DEPTH_SIZE = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_DEPTH_SIZE", 24U );

        if( SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, Engine_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE to [%u] error: %s"
                , Engine_SDL_GL_DEPTH_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DOUBLEBUFFER = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_DOUBLEBUFFER", 1U );

        if( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Engine_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DOUBLEBUFFER to [%u] error: %s"
                , Engine_SDL_GL_DOUBLEBUFFER
                , SDL_GetError()
            );
        }
#endif

#if defined(MENGINE_PLATFORM_UWP)
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "direct3d11" );

#else
        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengl" );

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_CORE );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_CONTEXT_MAJOR_VERSION", 3U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE_INTEGER( "SDL2", "SDL_GL_CONTEXT_MINOR_VERSION", 2U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        PathString Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE_PATHSTRING( "SDL2", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY.c_str() ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY.c_str()
                , SDL_GetError()
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

#if defined(MENGINE_PLATFORM_UWP)
       //Empty
#else
        SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );
#endif

        Uint32 windowFlags = 0;

#if defined(MENGINE_PLATFORM_UWP)
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

        LOGGER_INFO( "platform", "num video displays: %d"
            , SDL_GetNumVideoDisplays()
        );
        
        const Char * projectTitle_str = m_projectTitle.c_str();

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

            window = SDL_CreateWindow( projectTitle_str
                , window_x_mode
                , window_y_mode
                , width
                , height
                , windowFlags
            );
        }
        else
        {
            window = SDL_CreateWindow( projectTitle_str
                , SDL_WINDOWPOS_UNDEFINED
                , SDL_WINDOWPOS_UNDEFINED
                , mode.w
                , mode.h
                , windowFlags
            );
        }

        if( window == nullptr )
        {
            LOGGER_ERROR( "create window failed: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_sdlWindow = window;

        LOGGER_INFO( "platform", "SDL_HINT_RENDER_DRIVER: %s", SDL_GetHint( SDL_HINT_RENDER_DRIVER ) );
        LOGGER_INFO( "platform", "SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetHint( SDL_HINT_RENDER_SCALE_QUALITY ) );
        LOGGER_INFO( "platform", "SDL_HINT_ORIENTATIONS: %s", SDL_GetHint( SDL_HINT_ORIENTATIONS ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * getPlatformEventMessage( Uint32 _eventId )
        {
            switch( _eventId )
            {
                /* Application events */
                MENGINE_MESSAGE_CASE( SDL_QUIT, "User - requested quit" );

                /* These application events have special meaning on iOS, see README-ios.md for details */
                MENGINE_MESSAGE_CASE( SDL_APP_TERMINATING, "The application is being terminated by the OS Called on iOS in applicationWillTerminate Called on Android in onDestroy" );
                MENGINE_MESSAGE_CASE( SDL_APP_LOWMEMORY, "The application is low on memory, free memory if possible.Called on iOS in applicationDidReceiveMemoryWarning Called on Android in onLowMemory" );
                MENGINE_MESSAGE_CASE( SDL_APP_WILLENTERBACKGROUND, "The application is about to enter the background Called on iOS in applicationWillResignActive Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_APP_DIDENTERBACKGROUND, "The application did enter the background and may not get CPU for some time Called on iOS in applicationDidEnterBackground Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_APP_WILLENTERFOREGROUND, "The application is about to enter the foreground Called on iOS in applicationWillEnterForeground Called on Android in onResume" );
                MENGINE_MESSAGE_CASE( SDL_APP_DIDENTERFOREGROUND, "The application is now interactive Called on iOS in applicationDidBecomeActive Called on Android in onResume" );
                
                MENGINE_MESSAGE_CASE( SDL_LOCALECHANGED, "The user's locale preferences have changed." );

                /* Display events */
                MENGINE_MESSAGE_CASE( SDL_DISPLAYEVENT, "Display state change" );

                /* Window events */
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT, "Window state change" );
                MENGINE_MESSAGE_CASE( SDL_SYSWMEVENT, "System specific event" );

                /* Keyboard events */
                MENGINE_MESSAGE_CASE( SDL_KEYDOWN, "Key pressed" );
                MENGINE_MESSAGE_CASE( SDL_KEYUP, "Key released" );
                MENGINE_MESSAGE_CASE( SDL_TEXTEDITING, "Keyboard text editing (composition)" );
                MENGINE_MESSAGE_CASE( SDL_TEXTINPUT, "Keyboard text input" );
                MENGINE_MESSAGE_CASE( SDL_KEYMAPCHANGED, "Keymap changed due to a system event such as an input language or keyboard layout change." );

                MENGINE_MESSAGE_CASE( SDL_TEXTEDITING_EXT, "Extended keyboard text editing (composition)" );

                /* Mouse events */
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
            default:
                break;
            }

            if( _eventId >= SDL_USEREVENT )
            {
                static MENGINE_THREAD_LOCAL Char userEventMessage[32 + 1] = {'\0'};
                MENGINE_SNPRINTF( userEventMessage, 32, "[User event: %u]"
                    , _eventId
                );

                return userEventMessage;
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
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_ICCPROF_CHANGED, "The ICC profile of the window's display has changed." );
                MENGINE_MESSAGE_CASE( SDL_WINDOWEVENT_DISPLAY_CHANGED, "Window has been moved to display data1." );
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2PlatformService::processEvents_()
    {
        bool shouldQuit = false;
        
        SDL_Event sdlEvent;
        while( SDL_PollEvent( &sdlEvent ) != 0 )
        {
            LOGGER_INFO( "platform", "platform event: %s (%u)"
                , Detail::getPlatformEventMessage( sdlEvent.type )
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
                    SDL_Keycode keyCode = sdlEvent.key.keysym.sym;
                    Uint16 keyMod = sdlEvent.key.keysym.mod;

                    switch( keyCode )
                    {
                    case SDLK_RETURN:
                        {
                            if( keyMod & KMOD_ALT )
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
                        , Detail::getWindowEventMessage( windowEventId )
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
                            Sint32 width = sdlEvent.window.data1;
                            Sint32 height = sdlEvent.window.data2;

                            Resolution windowResolution( width, height );

                            APPLICATION_SERVICE()
                                ->setWindowResolution( windowResolution );
                        }break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            Sint32 width = sdlEvent.window.data1;
                            Sint32 height = sdlEvent.window.data2;

                            MENGINE_UNUSED( width );
                            MENGINE_UNUSED( height );
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
                            this->pushQuitEvent_();
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
                    shouldQuit = true;
                }break;
            default:
                break;
            }
        }

        if( shouldQuit == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::pushQuitEvent_()
    {
        SDL_Event e;
        e.type = SDL_QUIT;
        e.quit.timestamp = SDL_GetTicks();

        if( SDL_PushEvent( &e ) == -1 )
        {
            LOGGER_WARNING( "invalid push event [SDL_QUIT] error: %s"
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2PlatformService::setActive_( bool _active )
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
    bool SDL2PlatformService::isNeedWindowRender() const
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
    bool SDL2PlatformService::initializeFileService()
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
    void SDL2PlatformService::finalizeFileService()
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
}
