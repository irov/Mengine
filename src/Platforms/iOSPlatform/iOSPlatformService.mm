#include "iOSPlatformService.h"

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

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSKernelServiceInterface.h"

#include "iOSAnalyticsEventProvider.h"

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
#ifndef SDL_IPHONE_MAX_GFORCE
#define SDL_IPHONE_MAX_GFORCE 5.0f
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::iOSPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSPlatformService::iOSPlatformService()
        : m_beginTime( 0 )
        , m_sdlWindow( nullptr )
        , m_sdlAccelerometer( nullptr )
        , m_sdlInput( nullptr )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_sleepMode( true )
        , m_freezedTick( 0 )
        , m_freezedRender( 0 )
        , m_desktop( false )
        , m_touchpad( false )
        , m_glContext( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSPlatformService::~iOSPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSPlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        NSString * path = [[NSBundle mainBundle] resourcePath];
        
        StdString::strcpy_safe( _currentPath, [path UTF8String], MENGINE_MAX_PATH );
        StdString::strchrcat_safe( _currentPath, MENGINE_PATH_FORWARDSLASH, MENGINE_MAX_PATH );

        size_t path_length = [path length] + 1;
        
        return path_length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSPlatformService::getUserPath( Char * const _userPath ) const
    {
        const Char * Project_Company = CONFIG_VALUE_STRING( "Project", "Company", "UNKNOWN" );
        const Char * Project_Name = CONFIG_VALUE_STRING( "Project", "Name", "UNKNOWN" );

        NSArray * paths = NSSearchPathForDirectoriesInDomains( NSApplicationSupportDirectory, NSUserDomainMask, YES );
        
        if( [paths count] == 0 )
        {
            StdString::strcpy_safe( _userPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        NSString * first_path = [paths objectAtIndex:0];

        NSString * userPath = [first_path stringByAppendingPathComponent:@(Project_Company)];
        userPath = [userPath stringByAppendingPathComponent:@(Project_Name)];
        
        Path extraPreferencesFolderName = {'\0'};
        size_t ExtraPreferencesFolderNameLen = this->getExtraPreferencesFolderName( extraPreferencesFolderName );

        if( ExtraPreferencesFolderNameLen != 0 )
        {
            userPath = [userPath stringByAppendingPathComponent:@(extraPreferencesFolderName)];
        }

        NSError * error = nil;
        [[NSFileManager defaultManager] createDirectoryAtPath:userPath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:&error];

        if( error != nil )
        {
            IOS_LOGGER_ERROR( @"Error creating directory: %@"
                 , [AppleDetail getMessageFromNSError:error]
            );

            StdString::strcpy_safe( _userPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        const Char * userPathRepresentation = [userPath fileSystemRepresentation];
        
        StdString::strcpy_safe( _userPath, userPathRepresentation, MENGINE_MAX_PATH );
        Helper::pathCorrectFolderPathA( _userPath, MENGINE_PATH_FORWARDSLASH );
        Helper::pathCorrectBackslashA( _userPath );

        size_t userPathLen = StdString::strlen( _userPath );

        return userPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        const Char * Project_ExtraPreferencesFolderName = CONFIG_VALUE_STRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( Project_ExtraPreferencesFolderName ) == true, "invalid extra preferences folder name '%s'"
            , Project_ExtraPreferencesFolderName
        );

        StdString::strcpy_safe( _folderName, Project_ExtraPreferencesFolderName, MENGINE_MAX_PATH );
        
        size_t Project_ExtraPreferencesFolderNameLen = StdString::strlen( Project_ExtraPreferencesFolderName );

        return Project_ExtraPreferencesFolderNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        NSString * localeLanguage = [[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode];
        
        if( localeLanguage == nil )
        {
            return false;
        }
        
        const Char * localeLanguage_str = [localeLanguage UTF8String];

        _userLocaleLanguage[0] = localeLanguage_str[0];
        _userLocaleLanguage[1] = localeLanguage_str[1];
        _userLocaleLanguage[2] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t iOSPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        UIScreen * mainScreen = [UIScreen mainScreen];
        
        if( mainScreen == nil )
        {
            LOGGER_ERROR( "unable to access main screen" );
            
            return false;
        }

        CGRect screenRect = mainScreen.bounds;
        
        *_resolution = Resolution( screenRect.size.width, screenRect.size.height );
        
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
    bool iOSPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        SDL_version ver;
        SDL_GetVersion( &ver );

        LOGGER_INFO( "platform", "SDL version: %u.%u.%u"
            , ver.major
            , ver.minor
            , ver.patch
        );

        const char * revision = SDL_GetRevision();
        
        MENGINE_UNUSED( revision );

        LOGGER_INFO( "platform", "SDL revision: %s"
            , revision
        );

        SDL_bool isTablet = SDL_IsTablet();

        LOGGER_INFO( "platform", "SDL Tablet: %s"
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

        iOSInputPtr sdlInput = Helper::makeFactorableUnique<iOSInput>( MENGINE_DOCUMENT_FACTORABLE );

        if( sdlInput->initialize() == false )
        {
            return false;
        }

        m_sdlInput = sdlInput;

        uint64_t deviceSeed = Helper::generateRandomDeviceSeed();
        
        MENGINE_UNUSED( deviceSeed );

        LOGGER_INFO_PROTECTED( "platform", "Device Seed: %" MENGINE_PRIu64
            , deviceSeed
        );
        
        m_analyticsEventProvider = Helper::makeFactorableUnique<iOSAnalyticsEventProvider>( MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( m_analyticsEventProvider );
        
        m_mainScreenScale = [UIScreen mainScreen].scale;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::_runService()
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
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::_stopService()
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
    void iOSPlatformService::_finalizeService()
    {
        if( m_analyticsEventProvider != nullptr )
        {
            ANALYTICS_SERVICE()
                ->removeEventProvider( m_analyticsEventProvider );

            m_analyticsEventProvider = nullptr;
        }
        
        m_active = false;

        this->destroyWindow_();

        if( m_sdlInput != nullptr )
        {
            m_sdlInput->finalize();
            m_sdlInput = nullptr;
        }   

        m_platformTags.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::runPlatform()
    {        
        this->setActive_( true );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f, false, false, false );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause )
    {
        MENGINE_UNUSED( _pause );
        
        bool updating = APPLICATION_SERVICE()
            ->beginUpdate( _frameTime );

        if( m_freezedTick == 0 && updating == true )
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

        if( m_freezedRender == 0 && this->isNeedWindowRender() == true && _render == true )
        {
            bool sucessful = APPLICATION_SERVICE()
                ->render();

            if( sucessful == true && _flush == true )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::loopPlatform()
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

            this->tickPlatform( frameTime, true, true, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        NSURL * ns_url = [NSURL URLWithString:@(_url)];

        if( [[UIApplication sharedApplication] canOpenURL:ns_url] == NO )
        {
            return false;
        }

        [[UIApplication sharedApplication] openURL:ns_url
                                           options:@{}
                                 completionHandler:^(BOOL success) {
            //ToDo callback
        }];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        LOGGER_INFO( "platform", "open mail '%s' subject '%s' body '%s'"
            , _email
            , _subject
            , _body
        );

        UIViewController * viewController = [iOSDetail getRootViewController];

        if( [MFMailComposeViewController canSendMail] == NO )
        {
            [iOSDetail showOkAlertWithViewController:viewController
                                         title:@"Yikes."
                                       message:@"Log into your mailbox using the standard Mail app"
                                            ok:^{}];

            return false;
        }

        MFMailComposeViewController * mailCompose = [[MFMailComposeViewController alloc] init];

        [mailCompose setModalPresentationStyle:UIModalPresentationFormSheet];

        static iOSMailComposeDelegate * mailComposeDelegate = [[iOSMailComposeDelegate alloc] initWithCompletion:^ {
            //ToDo callback
        }];

        [mailCompose setMailComposeDelegate:mailComposeDelegate];

        [mailCompose setToRecipients:[NSArray arrayWithObjects:@(_email), nil]];
        [mailCompose setSubject:@(_subject)];
        [mailCompose setMessageBody:@(_body) isHTML:NO];

        [viewController presentViewController:mailCompose
                                     animated:YES
                                   completion:^ {
            //ToDo callback
        }] ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::openDeleteAccount()
    {
        LOGGER_INFO( "platform", "open delete account" );
        
        [iOSDetail showAreYouSureAlertDialogWithTitle:@"Delete Account"
                                              message:@"Click 'YES' will delete all account data. All game progress, virtual goods, and currency will be permanently removed and unrecoverable."
                                                delay:3000
                                                  yes:^() {
            LOGGER_MESSAGE("delete account [YES]");
            
            [iOSApplication.sharedInstance removeUserData];
            
            ACCOUNT_SERVICE()
                ->deleteCurrentAccount();
            
            [iOSDetail showOkAlertWithTitle:@"Account Deleted"
                                    message:@"Account data has been deleted. The application will now close."
                                         ok:^() {
                APPLICATION_SERVICE()
                    ->quit();
            }];
        }
                                               cancel: ^() {
            LOGGER_MESSAGE("delete account [CANCEL]");
            
        }];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::freezePlatform( bool _tick, bool _render )
    {
        if( _tick == true )
        {
            ++m_freezedTick;
        }

        if( _render == true )
        {
            ++m_freezedRender;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::unfreezePlatform( bool _tick, bool _render )
    {
        if( _tick == true )
        {
            --m_freezedTick;
        }

        if( _render == true )
        {
            --m_freezedRender;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::setSleepMode( bool _sleepMode )
    {
        m_sleepMode = _sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getSleepMode() const
    {
        return m_sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp iOSPlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::setProjectTitle( const Char * _projectTitle )
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
    size_t iOSPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
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
    bool iOSPlatformService::applyWindow_()
    {
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

        LOGGER_INFO( "platform", "SDL drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );

        int win_width;
        int win_height;
        SDL_GetWindowSize( m_sdlWindow, &win_width, &win_height );

        LOGGER_INFO( "platform", "SDL window size [%d, %d]"
            , win_width
            , win_height
        );

        int win_min_width;
        int win_min_height;
        SDL_GetWindowMinimumSize( m_sdlWindow, &win_min_width, &win_min_height );

        LOGGER_INFO( "platform", "SDL window min size [%d, %d]"
            , win_min_width
            , win_min_height
        );

        int win_max_width;
        int win_max_height;
        SDL_GetWindowMaximumSize( m_sdlWindow, &win_max_width, &win_max_height );

        LOGGER_INFO( "platform", "SDL window max size [%d, %d]"
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
                    LOGGER_INFO( "platform", "SDL display bounds [%d, %d] size [%d, %d]"
                        , displayBounds.x
                        , displayBounds.y
                        , displayBounds.w
                        , displayBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL display [%d] bounds get error: %s"
                        , displayIndex
                        , SDL_GetError()
                    );
                }

                SDL_Rect usableBounds;
                if( SDL_GetDisplayUsableBounds( displayIndex, &usableBounds ) == 0 )
                {
                    LOGGER_INFO( "platform", "SDL display usable bounds [%d, %d] size [%d, %d]"
                        , usableBounds.x
                        , usableBounds.y
                        , usableBounds.w
                        , usableBounds.h
                    );
                }
                else
                {
                    LOGGER_WARNING( "SDL display [%d] usable bounds get error: %s"
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
    bool iOSPlatformService::attachWindow( const void * _hWND )
    {
        this->setupWindow_();

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & iOSPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t iOSPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return static_cast<int32_t>(_dp * m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t iOSPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        return static_cast<int32_t>(_dp * m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t iOSPlatformService::pxToWidthDp( int32_t _px ) const
    {
        return static_cast<int32_t>(_px / m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t iOSPlatformService::pxToHeightDp( int32_t _px ) const
    {
        return static_cast<int32_t>(_px / m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr iOSPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );
        
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        uint32_t width;
        uint32_t height;

        int drawable_width;
        int drawable_height;
        SDL_GL_GetDrawableSize( m_sdlWindow, &drawable_width, &drawable_height );

        width = (uint32_t)drawable_width;
        height = (uint32_t)drawable_height;

        * _resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getAlwaysFullscreen() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::isDebuggerPresent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::debugBreak()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::closeWindow()
    {
        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::minimizeWindow()
    {
        SDL_MinimizeWindow( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::setCursorPosition( const mt::vec2f & _pos )
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
    void iOSPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::showKeyboard()
    {
        SDL_StartTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::hideKeyboard()
    {
        SDL_StopTextInput();
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::isShowKeyboard() const
    {
        SDL_bool active = SDL_IsTextInputActive();

        return active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
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
    void iOSPlatformService::notifyVsyncChanged( bool _vsync )
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
    void iOSPlatformService::notifyCursorModeChanged( bool _mode )
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
    bool iOSPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        //MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
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
    bool iOSPlatformService::setClipboardText( const Char * _value ) const
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
    bool iOSPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
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
    SDL_Window * iOSPlatformService::getWindow() const
    {
        return m_sdlWindow;
    }
    //////////////////////////////////////////////////////////////////////////
    UIWindow * iOSPlatformService::getUIWindow() const
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );

        SDL_GetWindowWMInfo( m_sdlWindow, &wmInfo );

        UIWindow * uiwindow = wmInfo.info.uikit.window;

        return uiwindow;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext iOSPlatformService::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId iOSPlatformService::addSDLEventHandler( const LambdaSDLEventHandler & _handler )
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
    void iOSPlatformService::removeSDLEventHandler( UniqueId _handlerId )
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
    bool iOSPlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::setupWindow_()
    {
        uint32_t Engine_SDL_GL_RED_SIZE = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_RED_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_RED_SIZE, Engine_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_RED_SIZE to [%u] error: %s"
                , Engine_SDL_GL_RED_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_GREEN_SIZE = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_GREEN_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, Engine_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_GREEN_SIZE to [%u] error: %s"
                , Engine_SDL_GL_GREEN_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_BLUE_SIZE = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_BLUE_SIZE", 8U );

        if( SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, Engine_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_BLUE_SIZE to [%u] error: %s"
                , Engine_SDL_GL_BLUE_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_ALPHA_SIZE = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_ALPHA_SIZE", 0U );

        if( SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, Engine_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_ALPHA_SIZE to [%u] error: %s"
                , Engine_SDL_GL_ALPHA_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DEPTH_SIZE = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_DEPTH_SIZE", 24U );

        if( SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, Engine_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE to [%u] error: %s"
                , Engine_SDL_GL_DEPTH_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DOUBLEBUFFER = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_DOUBLEBUFFER", 1U );

        if( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Engine_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DOUBLEBUFFER to [%u] error: %s"
                , Engine_SDL_GL_DOUBLEBUFFER
                , SDL_GetError()
            );
        }

        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengles2" );

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_ES );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE_INTEGER( "SDL", "SDL_GL_CONTEXT_MAJOR_VERSION", 2U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE_INTEGER( "SDL", "Engine_SDL_GL_CONTEXT_MINOR_VERSION", 0U );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE_STRING( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_ORIENTATIONS = CONFIG_VALUE_STRING( "SDL", "SDL_HINT_ORIENTATIONS", "Portrait" );

        if( SDL_SetHint( SDL_HINT_ORIENTATIONS, Engine_SDL_HINT_ORIENTATIONS ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_ORIENTATIONS to [%s] error: %s"
                , Engine_SDL_HINT_ORIENTATIONS
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR = CONFIG_VALUE_STRING( "SDL", "SDL_HINT_IOS_HIDE_HOME_INDICATOR", "1" );

        if( SDL_SetHint( SDL_HINT_IOS_HIDE_HOME_INDICATOR, Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_IOS_HIDE_HOME_INDICATOR to [%s] error: %s"
                , Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR
                , SDL_GetError()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

        Uint32 windowFlags = 0;

        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_SHOWN;
        windowFlags |= SDL_WINDOW_RESIZABLE;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;

        LOGGER_INFO( "platform", "num video displays: %d"
            , SDL_GetNumVideoDisplays()
        );

        const Char * projectTitle_str = m_projectTitle.c_str();

        SDL_Window * window = SDL_CreateWindow( projectTitle_str
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

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
    void iOSPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        if( m_glContext != nullptr )
        {
            SDL_GL_DeleteContext( m_glContext );
            m_glContext = nullptr;
        }

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
    bool iOSPlatformService::processEvents_()
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
            case SDL_APP_TERMINATING:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
                }break;
            case SDL_APP_LOWMEMORY:
                {                    
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
                }break;
            case SDL_APP_WILLENTERBACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
                }break;
            case SDL_APP_DIDENTERBACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
                }break;
            case SDL_APP_WILLENTERFOREGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
                }break;
            case SDL_APP_DIDENTERFOREGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
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
    void iOSPlatformService::pushQuitEvent_()
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
    void iOSPlatformService::setActive_( bool _active )
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
    bool iOSPlatformService::isNeedWindowRender() const
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
    bool iOSPlatformService::initializeFileService()
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
    void iOSPlatformService::finalizeFileService()
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
