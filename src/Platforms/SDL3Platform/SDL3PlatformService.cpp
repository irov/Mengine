#include "SDL3PlatformService.h"

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

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#elif defined(MENGINE_PLATFORM_APPLE)
#   if defined(MENGINE_PLATFORM_MACOS)
#       include "Environment/MacOS/MacOSUtils.h"
#   endif
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32Helper.h"
#   include "Environment/Windows/Win32FileHelper.h"
#endif

#include "SDL3DynamicLibrary.h"

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
SERVICE_FACTORY( PlatformService, Mengine::SDL3PlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3PlatformService::SDL3PlatformService()
        : m_beginTime( 0 )
        , m_sdlWindow( nullptr )
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
    SDL3PlatformService::~SDL3PlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t SDL3PlatformService::getCurrentPath( Char * const _currentPath ) const
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
    size_t SDL3PlatformService::getUserPath( Char * const _userPath ) const
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
    size_t SDL3PlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
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
    bool SDL3PlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        int count = 0;
        SDL_Locale ** locales = SDL_GetPreferredLocales( &count );
        
        if( locales == nullptr || count == 0 )
        {
            LOGGER_ERROR( "invalid get preferred locales: %s"
                , SDL_GetError()
            );

            return false;
        }

        _userLocaleLanguage[0] = (Char)locales[0]->language[0];
        _userLocaleLanguage[1] = (Char)locales[0]->language[1];
        _userLocaleLanguage[2] = '\0';

        SDL_free( locales );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDL3PlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        SDL_DisplayID displayID = 0;
        
        if( m_sdlWindow == nullptr )
        {
            int numDisplays = 0;
            SDL_DisplayID * displays = SDL_GetDisplays( &numDisplays );
            if( displays == nullptr || numDisplays == 0 )
            {
                LOGGER_ERROR( "invalid get displays: %s"
                    , SDL_GetError()
                );
                return false;
            }
            displayID = displays[0];
            SDL_free( displays );
        }
        else
        {
            displayID = SDL_GetDisplayForWindow( m_sdlWindow );

            if( displayID == 0 )
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

        SDL_Rect rect;
        if( SDL_GetDisplayUsableBounds( displayID, &rect ) == false )
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
        static bool SDL_EventFilter_RemoveMouse( void * userdata, SDL_Event * event )
        {
            MENGINE_UNUSED( userdata );

            switch( event->type )
            {
            case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    return false;
                }break;
            default: break;
            };

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool SDL_EventFilter_EnterBackground( void * userdata, SDL_Event * event )
        {
            MENGINE_UNUSED( userdata );

            switch( event->type )
            {
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
                {
                    return false;
                }break;
            case SDL_EVENT_DID_ENTER_BACKGROUND:
                {
                    return false;
                }break;
            default: break;
            };

            return true;
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
    bool SDL3PlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        int version = SDL_GetVersion();

        const char * revision = SDL_GetRevision();

        LOGGER_INFO( "platform", "SDL3 version: %d.%d.%d revision: %s"
            , SDL_VERSIONNUM_MAJOR( version )
            , SDL_VERSIONNUM_MINOR( version )
            , SDL_VERSIONNUM_MICRO( version )
            , revision
        );

        bool isTablet = SDL_IsTablet();

        LOGGER_INFO( "platform", "SDL3 Tablet: %s"
            , isTablet == true ? "true" : "false"
        );

#if defined(MENGINE_DEBUG)
        SDL_SetLogPriorities( SDL_LOG_PRIORITY_VERBOSE );
#else
        SDL_SetLogPriorities( SDL_LOG_PRIORITY_ERROR );
#endif

        SDL_SetLogOutputFunction( &Detail::SDL_LogOutputFunction, nullptr );

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

                if( isTablet == true )
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

                if( isTablet == true )
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

        SDL3InputPtr sdlInput = Helper::makeFactorableUnique<SDL3Input>( MENGINE_DOCUMENT_FACTORABLE );

        if( sdlInput->initialize() == false )
        {
            return false;
        }

        m_sdlInput = sdlInput;

        m_factoryDynamicLibraries = Helper::makeFactoryPool<SDL3DynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::_runService()
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
            , SDL_GetNumLogicalCPUCores()
            , SDL_GetCPUCacheLineSize()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU AltiVec: %d"
            , SDL_HasAltiVec()
        );

        LOGGER_INFO_PROTECTED( "platform", "  CPU MMX: %d"
            , SDL_HasMMX()
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

        // SDL_Joystick is deprecated in SDL3, accelerometer support removed
        LOGGER_INFO( "platform", "accelerometer support removed in SDL3" );

#if defined(MENGINE_PLATFORM_MACOS)
        //cppcheck-suppress syntaxError
        m_macOSWorkspace = [[MacOSWorkspace alloc] initialize];        
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::_stopService()
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

        // SDL_Joystick is deprecated in SDL3, accelerometer support removed
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::_finalizeService()
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
    bool SDL3PlatformService::runPlatform()
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
    void SDL3PlatformService::tickPlatform( float _frameTime )
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
    bool SDL3PlatformService::renderPlatform()
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
    void SDL3PlatformService::loopPlatform()
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
    bool SDL3PlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_ERROR( "not supported open url in default browser '%s'"
            , _url
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        LOGGER_ERROR( "not supported open mail '%s' subject '%s' body '%s'"
            , _email
            , _subject
            , _body
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::openDeleteAccount()
    {
        LOGGER_INFO( "platform", "open delete account" );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp SDL3PlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::setProjectTitle( const Char * _projectTitle )
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
    size_t SDL3PlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
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
    bool SDL3PlatformService::applyWindow_()
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

        // SDL3: SDL_GL_GetAttribute returns bool (true on success, false on error)
        int attribute_GL_CONTEXT_PROFILE_MASK = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &attribute_GL_CONTEXT_PROFILE_MASK ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_PROFILE_MASK error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MAJOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &attribute_GL_CONTEXT_MAJOR_VERSION ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MAJOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MINOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &attribute_GL_CONTEXT_MINOR_VERSION ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MINOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_RED_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &attribute_SDL_GL_RED_SIZE ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_RED_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_GREEN_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &attribute_SDL_GL_GREEN_SIZE ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_GREEN_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_BLUE_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &attribute_SDL_GL_BLUE_SIZE ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_BLUE_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_ALPHA_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &attribute_SDL_GL_ALPHA_SIZE ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_ALPHA_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DEPTH_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &attribute_SDL_GL_DEPTH_SIZE ) == false )
        {
            LOGGER_WARNING( "get attribute SDL_GL_DEPTH_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DOUBLEBUFFER = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &attribute_SDL_GL_DOUBLEBUFFER ) == false )
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
        SDL_GetWindowSizeInPixels( m_sdlWindow, &drawable_width, &drawable_height );

        LOGGER_INFO( "platform", "SDL3 drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );
#endif

        int win_width;
        int win_height;
        SDL_GetWindowSize( m_sdlWindow, &win_width, &win_height );

        LOGGER_INFO( "platform", "SDL3 window size [%d, %d]"
            , win_width
            , win_height
        );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        // SDL3: SDL_GetWindowBordersSize may not be supported on all platforms or window states
        int win_top = 0;
        int win_left = 0;
        int win_bottom = 0;
        int win_right = 0;
        SDL_GetWindowBordersSize( m_sdlWindow, &win_top, &win_left, &win_bottom, &win_right );

        LOGGER_INFO( "platform", "SDL3 window borders [%d, %d] - [%d, %d]"
            , win_left
            , win_top
            , win_right
            , win_bottom
        );
#endif

        int win_min_width;
        int win_min_height;
        SDL_GetWindowMinimumSize( m_sdlWindow, &win_min_width, &win_min_height );

        LOGGER_INFO( "platform", "SDL3 window min size [%d, %d]"
            , win_min_width
            , win_min_height
        );

        int win_max_width;
        int win_max_height;
        SDL_GetWindowMaximumSize( m_sdlWindow, &win_max_width, &win_max_height );

        LOGGER_INFO( "platform", "SDL3 window max size [%d, %d]"
            , win_max_width
            , win_max_height
        );

        SDL_WindowFlags flags = SDL_GetWindowFlags( m_sdlWindow );

        if( (flags & SDL_WINDOW_FULLSCREEN) != SDL_WINDOW_FULLSCREEN )
        {
            SDL_DisplayID displayID = SDL_GetDisplayForWindow( m_sdlWindow );

            if( displayID != 0 )
            {
                SDL_Rect displayBounds;
                if( SDL_GetDisplayBounds( displayID, &displayBounds ) == true )
                {
                    LOGGER_INFO( "platform", "SDL3 display bounds [%d, %d] size [%d, %d]"
                        , displayBounds.x
                        , displayBounds.y
                        , displayBounds.w
                        , displayBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL3 display bounds get error: %s"
                        , SDL_GetError()
                    );
                }

                SDL_Rect usableBounds;
                if( SDL_GetDisplayUsableBounds( displayID, &usableBounds ) == true )
                {
                    LOGGER_INFO( "platform", "SDL3 display usable bounds [%d, %d] size [%d, %d]"
                        , usableBounds.x
                        , usableBounds.y
                        , usableBounds.w
                        , usableBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL3 display usable bounds get error: %s"
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
    bool SDL3PlatformService::attachWindow( void * _hWND )
    {
        this->setupWindow_();

#if defined(MENGINE_PLATFORM_WINDOWS)
        // Create a temporary window to get pixel format
        SDL_Window * sharePixelFormatWindow = SDL_CreateWindow( "MengineSharePixelFormatWindow", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );

        if( sharePixelFormatWindow == nullptr )
        {
            LOGGER_ERROR( "create share pixel format window failed: %s"
                , SDL_GetError()
            );

            return false;
        }

        // Get HWND of the temporary window
        SDL_PropertiesID windowProps = SDL_GetWindowProperties( sharePixelFormatWindow );

        void * sharePixelFormatHWND = SDL_GetPointerProperty( windowProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr );

        if( sharePixelFormatHWND == nullptr )
        {
            LOGGER_ERROR( "get share pixel format window HWND failed: %s"
                , SDL_GetError()
            );

            SDL_DestroyWindow( sharePixelFormatWindow );

            return false;
        }

        // Create properties for window creation
        SDL_PropertiesID props = SDL_CreateProperties();

        if( props == 0 )
        {
            LOGGER_ERROR( "create properties failed: %s"
                , SDL_GetError()
            );

            SDL_DestroyWindow( sharePixelFormatWindow );

            return false;
        }

        // Set pixel format HWND property
        SDL_SetPointerProperty( props, SDL_PROP_WINDOW_CREATE_WIN32_PIXEL_FORMAT_HWND_POINTER, sharePixelFormatHWND );

        // Set main HWND property
        SDL_SetPointerProperty( props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, _hWND );

        // Create window from existing HWND
        SDL_Window * sdlWindow = SDL_CreateWindowWithProperties( props );

        // Clean up temporary window and properties
        SDL_DestroyWindow( sharePixelFormatWindow );
        SDL_DestroyProperties( props );

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
    bool SDL3PlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & SDL3PlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL3PlatformService::dpToWidthPx( int32_t _dp ) const
    {
        MENGINE_UNUSED( _dp );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL3PlatformService::dpToHeightPx( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL3PlatformService::pxToWidthDp( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t SDL3PlatformService::pxToHeightDp( int32_t _px ) const
    {
        MENGINE_UNUSED( _px );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr SDL3PlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        SDL3DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

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
    bool SDL3PlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        uint32_t width;
        uint32_t height;

        SDL_DisplayID displayID;

        if( m_sdlWindow == nullptr )
        {
            int numDisplays = 0;
            SDL_DisplayID * displays = SDL_GetDisplays( &numDisplays );
            if( displays == nullptr || numDisplays == 0 )
            {
                LOGGER_ERROR( "invalid get displays: %s"
                    , SDL_GetError()
                );
                return false;
            }
            displayID = displays[0];
            SDL_free( displays );
        }
        else
        {
            displayID = SDL_GetDisplayForWindow( m_sdlWindow );

            if( displayID == 0 )
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

        const SDL_DisplayMode * dm = SDL_GetDesktopDisplayMode( displayID );
        if( dm == nullptr )
        {
            LOGGER_ERROR( "invalid get desktop resolution error: %s"
                , SDL_GetError()
            );

            return false;
        }

        width = (uint32_t)dm->w;
        height = (uint32_t)dm->h;

        * _resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::isDebuggerPresent() const
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
    void SDL3PlatformService::debugBreak()
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        ::DebugBreak();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::closeWindow()
    {
        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::minimizeWindow()
    {
        SDL_MinimizeWindow( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        Resolution resolution;
        if( this->getDesktopResolution( &resolution ) == false )
        {
            return;
        }

        float width = resolution.getWidthF();
        float height = resolution.getHeightF();

        float wndPosX = _pos.x * width;
        float wndPosY = _pos.y * height;

        // ! This function generates a mouse motion event !
        SDL_WarpMouseInWindow( m_sdlWindow, wndPosX, wndPosY );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::showKeyboard()
    {
        SDL_StartTextInput( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::hideKeyboard()
    {
        SDL_StopTextInput( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::isShowKeyboard() const
    {
        bool active = SDL_TextInputActive( m_sdlWindow );

        return active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_sdlWindow == nullptr )
        {
            return true;
        }

        SDL_WindowFlags flags = SDL_GetWindowFlags( m_sdlWindow );

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
    void SDL3PlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        if( SDL_GetWindowFlags( m_sdlWindow ) & SDL_WINDOW_OPENGL )
        {
            int profile;
            if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &profile ) == false )
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
    void SDL3PlatformService::notifyCursorModeChanged( bool _mode )
    {
        if( _mode == true )
        {
            SDL_ShowCursor();
        }
        else
        {
            SDL_HideCursor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
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
    bool SDL3PlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
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

        SDL_IOStream * iostream = SDL_IOFromFile( path_file, "wb" );

        if( iostream == nullptr )
        {
            LOGGER_ERROR( "invalid create file '%s'"
                , path_file
            );

            return false;
        }

        size_t written = SDL_WriteIO( iostream, _data, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size %zu [error: %s]"
                , path_file
                , _size
                , SDL_GetError()
            );

            SDL_CloseIO( iostream );
            return false;
        }

        SDL_CloseIO( iostream );

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
    bool SDL3PlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
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

        SDL_IOStream * iostream = SDL_IOFromFile( path_file, "wb" );

        if( iostream == nullptr )
        {
            LOGGER_ERROR( "invalid create file '%s'"
                , path_file
            );

            return false;
        }

        size_t written = SDL_WriteIO( iostream, _data, _size );

        if( written != _size )
        {
            LOGGER_ERROR( "invalid write file '%s' size %zu [error: %s]"
                , path_file
                , _size
                , SDL_GetError()
            );

            SDL_CloseIO( iostream );
            return false;
        }

        SDL_CloseIO( iostream );

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
    void SDL3PlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
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
    bool SDL3PlatformService::setClipboardText( const Char * _value ) const
    {
        if( SDL_SetClipboardText( _value ) == false )
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
    bool SDL3PlatformService::getClipboardText( Char * _value, size_t _capacity ) const
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
    SDL_Window * SDL3PlatformService::getWindow() const
    {
        return m_sdlWindow;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
#   if defined(MENGINE_PLATFORM_UWP)
    //////////////////////////////////////////////////////////////////////////
    IInspectable * SDL3PlatformService::getWindowHandle() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * window = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_WINRT_CORE_WINDOW_POINTER, nullptr );
        
        IInspectable * iwindow = static_cast<IInspectable *>(window);

        return iwindow;
    }
    //////////////////////////////////////////////////////////////////////////
#   else
    //////////////////////////////////////////////////////////////////////////
    HWND SDL3PlatformService::getWindowHWND() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * hwnd = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr );

        return static_cast<HWND>(hwnd);
    }
    //////////////////////////////////////////////////////////////////////////
    HDC SDL3PlatformService::getWindowHDC() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * hdc = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr );

        return static_cast<HDC>(hdc);
    }
    //////////////////////////////////////////////////////////////////////////
    HINSTANCE SDL3PlatformService::getWindowHinstance() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * hinstance = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr );

        return static_cast<HINSTANCE>(hinstance);
    }
    //////////////////////////////////////////////////////////////////////////
#   endif
#elif defined(MENGINE_PLATFORM_LINUX)
    //////////////////////////////////////////////////////////////////////////
    Window SDL3PlatformService::getWindowInstance() const
    {
        // SDL3 3.4.0: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        // SDL_GetPointerProperty returns void*, but for X11 window number we need to get it as Uint64
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        Uint64 windowNumber = 0;
        void * windowNumberPtr = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr );
        if( windowNumberPtr != nullptr )
        {
            windowNumber = reinterpret_cast<Uint64>(windowNumberPtr);
        }

        return static_cast<Window>(windowNumber);
    }
    //////////////////////////////////////////////////////////////////////////
    Display * SDL3PlatformService::getWindowDisplay() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * display = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr );

        return static_cast<Display *>(display);
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_IOS)
    //////////////////////////////////////////////////////////////////////////
    UIWindow * SDL3PlatformService::getUIWindow() const
    {
        // SDL3: Use SDL_GetWindowProperties and SDL_GetPointerProperty
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * window = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr );

        return static_cast<UIWindow *>(window);
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext SDL3PlatformService::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    UniqueId SDL3PlatformService::addSDLEventHandler( const LambdaSDLEventHandler & _handler )
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
    void SDL3PlatformService::removeSDLEventHandler( UniqueId _handlerId )
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
    bool SDL3PlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
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
    void SDL3PlatformService::setupWindow_()
    {
#if defined(MENGINE_PLATFORM_UWP)
#else
        uint32_t Engine_SDL_GL_RED_SIZE = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_RED_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_RED_SIZE, Engine_SDL_GL_RED_SIZE ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_RED_SIZE to [%u] error: %s"
                , Engine_SDL_GL_RED_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_GREEN_SIZE = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_GREEN_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, Engine_SDL_GL_GREEN_SIZE ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_GREEN_SIZE to [%u] error: %s"
                , Engine_SDL_GL_GREEN_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_BLUE_SIZE = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_BLUE_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, Engine_SDL_GL_BLUE_SIZE ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_BLUE_SIZE to [%u] error: %s"
                , Engine_SDL_GL_BLUE_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_ALPHA_SIZE = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_ALPHA_SIZE", 0U );

        if( SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, Engine_SDL_GL_ALPHA_SIZE ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_ALPHA_SIZE to [%u] error: %s"
                , Engine_SDL_GL_ALPHA_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DEPTH_SIZE = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_DEPTH_SIZE", 24U );

        if( SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, Engine_SDL_GL_DEPTH_SIZE ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE to [%u] error: %s"
                , Engine_SDL_GL_DEPTH_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DOUBLEBUFFER = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_DOUBLEBUFFER", 1U );

        if( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Engine_SDL_GL_DOUBLEBUFFER ) == false )
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

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_CORE );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_CONTEXT_MAJOR_VERSION", 3U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE_INTEGER( "SDL3", "SDL_GL_CONTEXT_MINOR_VERSION", 2U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) == false )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
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
#else
        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_HIDDEN;
#endif

        if( _fullscreen == true )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        int numDisplays = 0;
        SDL_DisplayID * displays = SDL_GetDisplays( &numDisplays );

        LOGGER_INFO( "platform", "num video displays: %d"
            , numDisplays
        );

        if( displays != nullptr )
        {
            SDL_free( displays );
        }
        
        const Char * projectTitle_str = m_projectTitle.c_str();

        SDL_DisplayID displayID = 0;

        if( numDisplays > 0 )
        {
            displays = SDL_GetDisplays( &numDisplays );

            if( displays != nullptr && numDisplays > 0 )
            {
                displayID = displays[0];
                SDL_free( displays );
            }
        }

        const SDL_DisplayMode * mode = SDL_GetDesktopDisplayMode( displayID );

        if( mode == nullptr )
        {
            LOGGER_ERROR( "get desktop display mode error: %s"
                , SDL_GetError()
            );

            return false;
        }

        SDL_Window * window;

        if( _fullscreen == false )
        {
            uint32_t width = _windowResolution.getWidth();
            uint32_t height = _windowResolution.getHeight();

            window = SDL_CreateWindow( projectTitle_str
                , (int)width
                , (int)height
                , windowFlags
            );
        }
        else
        {
            window = SDL_CreateWindow( projectTitle_str
                , mode->w
                , mode->h
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
        LOGGER_INFO( "platform", "SDL_HINT_ORIENTATIONS: %s", SDL_GetHint( SDL_HINT_ORIENTATIONS ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
        if( m_glContext != nullptr )
        {
            SDL_GL_DestroyContext( m_glContext );
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
                MENGINE_MESSAGE_CASE( SDL_EVENT_QUIT, "User - requested quit" );

                /* These application events have special meaning on iOS, see README-ios.md for details */
                MENGINE_MESSAGE_CASE( SDL_EVENT_TERMINATING, "The application is being terminated by the OS Called on iOS in applicationWillTerminate Called on Android in onDestroy" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_LOW_MEMORY, "The application is low on memory, free memory if possible.Called on iOS in applicationDidReceiveMemoryWarning Called on Android in onLowMemory" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WILL_ENTER_BACKGROUND, "The application is about to enter the background Called on iOS in applicationWillResignActive Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DID_ENTER_BACKGROUND, "The application did enter the background and may not get CPU for some time Called on iOS in applicationDidEnterBackground Called on Android in onPause" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WILL_ENTER_FOREGROUND, "The application is about to enter the foreground Called on iOS in applicationWillEnterForeground Called on Android in onResume" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DID_ENTER_FOREGROUND, "The application is now interactive Called on iOS in applicationDidBecomeActive Called on Android in onResume" );
                
                MENGINE_MESSAGE_CASE( SDL_EVENT_LOCALE_CHANGED, "The user's locale preferences have changed." );

                MENGINE_MESSAGE_CASE( SDL_EVENT_SYSTEM_THEME_CHANGED, "The system theme changed" );

                /* Display events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_ORIENTATION, "Display orientation has changed to data1" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_ADDED, "Display has been added to the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_REMOVED, "Display has been removed from the system" );                
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_MOVED, "Display has changed position" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED, "Display has changed position" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED, "Display current mode has changed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED, "Display has changed content scale" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED, "Display has changed usable bounds" );

                /* Window events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_SHOWN, "Window has been shown" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_HIDDEN, "Window has been hidden" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_EXPOSED, "Window has been exposed and should be redrawn" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_MOVED, "Window has been moved to data1, data2" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_RESIZED, "Window has been resized to data1xdata2" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED, "The pixel size of the window has changed to data1xdata2" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_METAL_VIEW_RESIZED, "The pixel size of a Metal view associated with the window has changed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_MINIMIZED, "Window has been minimized" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_MAXIMIZED, "Window has been maximized" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_RESTORED, "Window has been restored to normal size and position" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_MOUSE_ENTER, "Window has gained mouse focus" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_MOUSE_LEAVE, "Window has lost mouse focus" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_FOCUS_GAINED, "Window has gained keyboard focus" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_FOCUS_LOST, "Window has lost keyboard focus" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_CLOSE_REQUESTED, "The window manager requests that the window be closed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_HIT_TEST, "Window had a hit test that wasn't SDL_HITTEST_NORMAL" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_ICCPROF_CHANGED, "The ICC profile of the window's display has changed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_DISPLAY_CHANGED, "Window has been moved to display data1" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED, "Window display scale has been changed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_SAFE_AREA_CHANGED, "The window safe area has been changed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_OCCLUDED, "The window has been occluded" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_ENTER_FULLSCREEN, "The window has entered fullscreen mode" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_LEAVE_FULLSCREEN, "The window has left fullscreen mode" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_DESTROYED, "The window has been destroyed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_HDR_STATE_CHANGED, "The window HDR state has changed" );

                /* Keyboard events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_KEY_DOWN, "Key pressed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_KEY_UP, "Key released" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_TEXT_EDITING, "Keyboard text editing (composition)" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_TEXT_INPUT, "Keyboard text input" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_KEYMAP_CHANGED, "Keymap changed due to a system event such as an input language or keyboard layout change." );
                MENGINE_MESSAGE_CASE( SDL_EVENT_KEYBOARD_ADDED, "A new keyboard has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_KEYBOARD_REMOVED, "A keyboard has been removed from the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_TEXT_EDITING_CANDIDATES, "Keyboard text editing candidates" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_SCREEN_KEYBOARD_SHOWN, "The on-screen keyboard has been shown" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_SCREEN_KEYBOARD_HIDDEN, "The on-screen keyboard has been hidden" );

                /* Mouse events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_MOTION, "Mouse moved" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_BUTTON_DOWN, "Mouse button pressed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_BUTTON_UP, "Mouse button released" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_WHEEL, "Mouse wheel motion" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_ADDED, "A new mouse has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_MOUSE_REMOVED, "A mouse has been removed from the system" );

                /* Joystick events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_AXIS_MOTION, "Joystick axis motion" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_BALL_MOTION, "Joystick trackball motion" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_HAT_MOTION, "Joystick hat position change" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_BUTTON_DOWN, "Joystick button pressed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_BUTTON_UP, "Joystick button released" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_ADDED, "A new joystick has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_REMOVED, "An opened joystick has been removed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_BATTERY_UPDATED, "Joystick battery level has been updated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_JOYSTICK_UPDATE_COMPLETE, "Joystick update complete" );                

                /* Gamepad events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_AXIS_MOTION, "Game controller axis motion" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_BUTTON_DOWN, "Game controller button pressed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_BUTTON_UP, "Game controller button released" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_ADDED, "A new Game controller has been inserted into the system" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_REMOVED, "An opened Game controller has been removed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_REMAPPED, "The controller mapping was updated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN, "Game controller touchpad was touched" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION, "Game controller touchpad finger was moved" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_TOUCHPAD_UP, "Game controller touchpad finger was lifted" ); 
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_SENSOR_UPDATE, "Game controller sensor was updated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_UPDATE_COMPLETE, "Gamepad update complete" );                
                MENGINE_MESSAGE_CASE( SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED, "Gamepad Steam handle has changed" );
                

                /* Touch events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_FINGER_DOWN, "SDL_FINGERDOWN" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_FINGER_UP, "SDL_FINGERUP" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_FINGER_MOTION, "SDL_FINGERMOTION" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_FINGER_CANCELED, "SDL_FINGERCANCELED" );

                /* Pinch events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_PINCH_BEGIN, "Pinch gesture started" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PINCH_UPDATE, "Pinch gesture updated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PINCH_END, "Pinch gesture ended" );

                /* Clipboard events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_CLIPBOARD_UPDATE, "The clipboard changed" );

                /* Drag and drop events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_DROP_FILE, "The system requests a file open" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DROP_TEXT, "text/plain drag-and-drop event" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DROP_BEGIN, "A new set of drops is beginning (NULL filename)" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DROP_COMPLETE, "Current set of drops is now complete (NULL filename)" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DROP_POSITION, "Position while moving over the window" );

                /* Audio hotplug events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_AUDIO_DEVICE_ADDED, "A new audio device is available" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_AUDIO_DEVICE_REMOVED, "An audio device has been removed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED, "An audio device's format has been changed by the system" );

                /* Sensor events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_SENSOR_UPDATE, "A sensor was updated" );

                /* Pressure-sensitive pen events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_PROXIMITY_IN, "Pressure-sensitive pen has become available" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_PROXIMITY_OUT, "Pressure-sensitive pen has become unavailable" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_DOWN, "Pressure-sensitive pen touched drawing surface" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_UP, "Pressure-sensitive pen stopped touching drawing surface" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_BUTTON_DOWN, "Pressure-sensitive pen button pressed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_BUTTON_UP, "Pressure-sensitive pen button released" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_MOTION, "Pressure-sensitive pen is moving on the tablet" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_PEN_AXIS, "Pressure-sensitive pen angle/pressure/etc changed" );

                /* Camera hotplug events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_CAMERA_DEVICE_ADDED, "A new camera device is available" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_CAMERA_DEVICE_REMOVED, "A camera device has been removed" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_CAMERA_DEVICE_APPROVED, "A camera device has been approved for use by the user" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_CAMERA_DEVICE_DENIED, "A camera device has been denied for use by the user" );

                /* Render events */
                MENGINE_MESSAGE_CASE( SDL_EVENT_RENDER_TARGETS_RESET, "The render targets have been reset and their contents need to be updated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_RENDER_DEVICE_RESET, "The device has been reset and all textures need to be recreated" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_RENDER_DEVICE_LOST, "The device has been lost and can't be recovered" );
            default:
                break;
            }

            if( _eventId >= SDL_EVENT_USER && _eventId <= SDL_EVENT_LAST )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3PlatformService::processEvents_()
    {
        bool shouldQuit = false;
        
        SDL_Event sdlEvent;
        while( SDL_PollEvent( &sdlEvent ) == true )
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
            case SDL_EVENT_KEY_DOWN:
                {
                    SDL_Keycode keyCode = sdlEvent.key.key;
                    Uint16 keyMod = sdlEvent.key.mod;

                    switch( keyCode )
                    {
                    case SDLK_RETURN:
                        {
                            if( keyMod & SDL_KMOD_ALT )
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
            case SDL_EVENT_WINDOW_SHOWN:
                {
                    //TODO
                }break;
            case SDL_EVENT_WINDOW_HIDDEN:
                {
                    //TODO
                }break;
            case SDL_EVENT_WINDOW_EXPOSED:
                {
                    //TODO
                }break;
            case SDL_EVENT_WINDOW_MOVED:
                {
                    //TODO
                }break;
            case SDL_EVENT_WINDOW_RESIZED:
                {
                    // SDL3 3.4.0: Use width and height fields directly
                    Sint32 width = sdlEvent.window.data1;
                    Sint32 height = sdlEvent.window.data2;

                    Resolution windowResolution( width, height );

                    APPLICATION_SERVICE()
                        ->setWindowResolution( windowResolution );
                }break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_MAXIMIZED:
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                {
                    this->setActive_( true );
                }break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            case SDL_EVENT_WINDOW_MINIMIZED:
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                {
                    this->setActive_( false );
                }break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    this->pushQuitEvent_();
                }break;
            case SDL_EVENT_QUIT:
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
    void SDL3PlatformService::pushQuitEvent_()
    {
        SDL_Event e;
        e.type = SDL_EVENT_QUIT;
        e.quit.timestamp = SDL_GetTicks();

        if( SDL_PushEvent( &e ) == false )
        {
            LOGGER_WARNING( "invalid push event [SDL_QUIT] error: %s"
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3PlatformService::setActive_( bool _active )
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
    bool SDL3PlatformService::isNeedWindowRender() const
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
    bool SDL3PlatformService::initializeFileService()
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
    void SDL3PlatformService::finalizeFileService()
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

