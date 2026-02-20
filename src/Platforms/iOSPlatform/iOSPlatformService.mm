#include "iOSPlatformService.h"

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
#include "Interface/SoundServiceInterface.h"
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
        , m_sdlInput( nullptr )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
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
        PathString Project_Company = CONFIG_VALUE_PATHSTRING( "Project", "Company", "UNKNOWN" );
        PathString Project_Name = CONFIG_VALUE_PATHSTRING( "Project", "Name", "UNKNOWN" );

        NSArray * paths = NSSearchPathForDirectoriesInDomains( NSApplicationSupportDirectory, NSUserDomainMask, YES );
        
        if( [paths count] == 0 )
        {
            StdString::strcpy_safe( _userPath, "", MENGINE_MAX_PATH );

            return 0;
        }

        NSString * userPath = [NSString pathWithComponents:@[[paths objectAtIndex:0], @(Project_Company.c_str()), @(Project_Name.c_str())]];
        
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
        PathString Project_ExtraPreferencesFolderName = CONFIG_VALUE_PATHSTRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( Project_ExtraPreferencesFolderName.c_str() ) == true, "invalid extra preferences folder name '%s'"
            , Project_ExtraPreferencesFolderName.c_str()
        );

        StdString::strcpy_safe( _folderName, Project_ExtraPreferencesFolderName.c_str(), MENGINE_MAX_PATH );
        
        size_t Project_ExtraPreferencesFolderNameLen = Project_ExtraPreferencesFolderName.size();

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
    bool iOSPlatformService::_initializeService()
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
            }
            else if( StdString::strcmp( sdlPlatform, "WinRT" ) == 0 )
            {
                m_desktop = true;
                m_touchpad = false;

                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
                m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WinRT" ) );

                SDL_SetEventFilter( &Detail::SDL_EventFilter_EnterBackground, nullptr );
            }
            else if( StdString::strcmp( sdlPlatform, "macOS" ) == 0 || StdString::strcmp( sdlPlatform, "Mac OS X" ) == 0 )
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

        // SDL_Joystick/accelerometer support removed in SDL3
        LOGGER_INFO( "platform", "accelerometer support removed in SDL3" );

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

        this->tickPlatform( 0.f );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::tickPlatform( float _frameTime )
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

        if( updating == false )
        {
            if( m_pauseUpdatingTime < 0.f )
            {
                m_pauseUpdatingTime = _frameTime;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::renderPlatform()
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

        if( m_sdlWindow == nullptr )
        {
            return false;
        }

        SDL_ShowWindow( m_sdlWindow );

        if( SDL_GetWindowFlags( m_sdlWindow ) & SDL_WINDOW_OPENGL )
        {
            SDL_GL_SwapWindow( m_sdlWindow );
        }
        
        return true;
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

        NSMutableString * mailBodyBuilder = [NSMutableString stringWithUTF8String:_body];

        [mailBodyBuilder appendString:@"\n\n"];
        [mailBodyBuilder appendString:@"----- Please Describe Your Message Above Here -----\n\n"];

        {
            Mengine::Char userName[MENGINE_ENVIRONMENT_USER_MAXNAME] = {'\0'};
            Mengine::Char deviceModel[MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME] = {'\0'};
            Mengine::Char deviceLanguage[MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME] = {'\0'};
            Mengine::Char osFamily[MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME] = {'\0'};
            Mengine::Char osVersion[MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME] = {'\0'};
            Mengine::Char bundleId[MENGINE_ENVIRONMENT_BUNDLEID_MAXNAME] = {'\0'};
            Mengine::Char installId[MENGINE_ENVIRONMENT_INSTALLID_MAXNAME] = {'\0'};
            Mengine::Char installVersion[MENGINE_ENVIRONMENT_INSTALLVERSION_MAXNAME] = {'\0'};
            Mengine::Char sessionId[MENGINE_ENVIRONMENT_SESSIONID_MAXNAME] = {'\0'};

            ENVIRONMENT_SERVICE()
                ->getUserName( userName );

            ENVIRONMENT_SERVICE()
                ->getDeviceModel( deviceModel );

            ENVIRONMENT_SERVICE()
                ->getDeviceLanguage( deviceLanguage );

            ENVIRONMENT_SERVICE()
                ->getOSFamily( osFamily );

            ENVIRONMENT_SERVICE()
                ->getOSVersion( osVersion );

            ENVIRONMENT_SERVICE()
                ->getBundleId( bundleId );

            ENVIRONMENT_SERVICE()
                ->getInstallId( installId );

            ENVIRONMENT_SERVICE()
                ->getInstallVersion( installVersion );

            ENVIRONMENT_SERVICE()
                ->getSessionId( sessionId );

            [mailBodyBuilder appendString:@"[Application]\n"];
            [mailBodyBuilder appendFormat:@"    User: %s\n", userName];
            [mailBodyBuilder appendFormat:@"    Bundle: %s\n", bundleId];
            [mailBodyBuilder appendFormat:@"    InstallId: %s\n", installId];
            [mailBodyBuilder appendFormat:@"    InstallVersion: %s\n", installVersion];
            [mailBodyBuilder appendFormat:@"    SessionId: %s\n", sessionId];
            [mailBodyBuilder appendString:@"\n"];

            [mailBodyBuilder appendString:@"[Device]\n"];
            [mailBodyBuilder appendFormat:@"    Model: %s\n", deviceModel];
            [mailBodyBuilder appendFormat:@"    Language: %s\n", deviceLanguage];
            [mailBodyBuilder appendFormat:@"    OSFamily: %s\n", osFamily];
            [mailBodyBuilder appendFormat:@"    OSVersion: %s\n", osVersion];
            [mailBodyBuilder appendString:@"\n"];
        }

        [mailCompose setMessageBody:mailBodyBuilder isHTML:NO];

        Mengine::Char userPath[MENGINE_MAX_PATH] = {'\0'};
        size_t userPathLen = this->getUserPath( userPath );

        if( userPathLen != 0 )
        {
            NSString * userPathString = [NSString stringWithUTF8String:userPath];

            NSFileManager * fileManager = [NSFileManager defaultManager];
            NSDirectoryEnumerator<NSString *> * enumerator = [fileManager enumeratorAtPath:userPathString];

            for( NSString * relativePath in enumerator )
            {
                NSString * fullPath = [userPathString stringByAppendingPathComponent:relativePath];

                BOOL isDirectory = NO;
                if( [fileManager fileExistsAtPath:fullPath isDirectory:&isDirectory] == NO || isDirectory == YES )
                {
                    continue;
                }

                NSString * extension = [[fullPath pathExtension] lowercaseString];

                if( [extension isEqualToString:@"log"] == NO &&
                    [extension isEqualToString:@"zip"] == NO &&
                    [extension isEqualToString:@"sav"] == NO )
                {
                    continue;
                }

                NSData * fileData = [NSData dataWithContentsOfFile:fullPath];

                if( fileData == nil )
                {
                    continue;
                }

                NSString * fileName = [relativePath lastPathComponent];

                NSString * mimeType = @"application/octet-stream";

                if( [extension isEqualToString:@"log"] == YES )
                {
                    mimeType = @"text/plain";
                }
                else if( [extension isEqualToString:@"zip"] == YES )
                {
                    mimeType = @"application/zip";
                }

                [mailCompose addAttachmentData:fileData
                                      mimeType:mimeType
                                      fileName:fileName];
            }
        }

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
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE error: %s"
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
    bool iOSPlatformService::attachWindow( void * _hWND )
    {
        this->setupWindow_();

        SDL_PropertiesID props = SDL_CreateProperties();
        if( props == 0 )
        {
            LOGGER_ERROR( "create properties failed: %s"
                , SDL_GetError()
            );
            return false;
        }

#if defined(SDL_PROP_WINDOW_CREATE_UIKIT_WINDOW_POINTER)
        SDL_SetPointerProperty( props, SDL_PROP_WINDOW_CREATE_UIKIT_WINDOW_POINTER, _hWND );
#else
        SDL_SetPointerProperty( props, SDL_PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER, _hWND );
#endif

        SDL_Window * sdlWindow = SDL_CreateWindowWithProperties( props );
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
        int drawable_width;
        int drawable_height;
        SDL_GetWindowSizeInPixels( m_sdlWindow, &drawable_width, &drawable_height );

        *_resolution = Resolution( (uint32_t)drawable_width, (uint32_t)drawable_height );

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
        SDL_StartTextInput( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::hideKeyboard()
    {
        SDL_StopTextInput( m_sdlWindow );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::isShowKeyboard() const
    {
        bool active = SDL_TextInputActive( m_sdlWindow );

        return active;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
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
    void iOSPlatformService::notifyVsyncChanged( bool _vsync )
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
                if( SDL_GL_SetSwapInterval( 0 ) == false )
                {
                    LOGGER_WARNING( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                if( SDL_GL_SetSwapInterval( 1 ) == false )
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
            SDL_ShowCursor();
        }
        else
        {
            SDL_HideCursor();
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
        SDL_PropertiesID props = SDL_GetWindowProperties( m_sdlWindow );
        void * window = nullptr;

#if defined(SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER)
        window = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr );
#else
        window = SDL_GetPointerProperty( props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr );
#endif

        return (__bridge UIWindow *)window;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext iOSPlatformService::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
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

#if defined(SDL_HINT_RENDER_SCALE_QUALITY)
        PathString Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE_PATHSTRING( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY.c_str() ) == false )
        {
            LOGGER_WARNING( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY.c_str()
                , SDL_GetError()
            );
        }
#endif

        PathString Engine_SDL_HINT_ORIENTATIONS = CONFIG_VALUE_PATHSTRING( "SDL", "SDL_HINT_ORIENTATIONS", "Portrait" );

        if( SDL_SetHint( SDL_HINT_ORIENTATIONS, Engine_SDL_HINT_ORIENTATIONS.c_str() ) == false )
        {
            LOGGER_WARNING( "set hint SDL_HINT_ORIENTATIONS to [%s] error: %s"
                , Engine_SDL_HINT_ORIENTATIONS.c_str()
                , SDL_GetError()
            );
        }

        PathString Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR = CONFIG_VALUE_PATHSTRING( "SDL", "SDL_HINT_IOS_HIDE_HOME_INDICATOR", "1" );

        if( SDL_SetHint( SDL_HINT_IOS_HIDE_HOME_INDICATOR, Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR.c_str() ) == false )
        {
            LOGGER_WARNING( "set hint SDL_HINT_IOS_HIDE_HOME_INDICATOR to [%s] error: %s"
                , Engine_SDL_HINT_IOS_HIDE_HOME_INDICATOR.c_str()
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
        windowFlags |= SDL_WINDOW_RESIZABLE;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;
        windowFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

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
        int width = ( mode != nullptr ) ? mode->w : 0;
        int height = ( mode != nullptr ) ? mode->h : 0;

        SDL_Window * window = SDL_CreateWindow( projectTitle_str
            , width
            , height
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
#if defined(SDL_HINT_RENDER_SCALE_QUALITY)
        LOGGER_INFO( "platform", "SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetHint( SDL_HINT_RENDER_SCALE_QUALITY ) );
#endif
        LOGGER_INFO( "platform", "SDL_HINT_ORIENTATIONS: %s", SDL_GetHint( SDL_HINT_ORIENTATIONS ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        if( m_glContext != nullptr )
        {
            SDL_GL_DestroyContext( m_glContext );
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
                MENGINE_MESSAGE_CASE( SDL_EVENT_QUIT, "User - requested quit" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_TERMINATING, "Application is being terminated by the OS" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_LOW_MEMORY, "Application is low on memory" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WILL_ENTER_BACKGROUND, "Application about to enter background" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DID_ENTER_BACKGROUND, "Application did enter background" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WILL_ENTER_FOREGROUND, "Application about to enter foreground" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_DID_ENTER_FOREGROUND, "Application is now interactive" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_RESIZED, "Window resized" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_FOCUS_GAINED, "Window focus gained" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_FOCUS_LOST, "Window focus lost" );
                MENGINE_MESSAGE_CASE( SDL_EVENT_WINDOW_CLOSE_REQUESTED, "Window close requested" );
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
    bool iOSPlatformService::processEvents_()
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

            if( sdlEvent.type >= SDL_EVENT_WINDOW_FIRST && sdlEvent.type <= SDL_EVENT_WINDOW_LAST )
            {
                if( sdlEvent.window.windowID != SDL_GetWindowID( m_sdlWindow ) )
                {
                    continue;
                }
            }

            switch( sdlEvent.type )
            {
            case SDL_EVENT_WINDOW_RESIZED:
                {
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
            case SDL_EVENT_TERMINATING:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
                }break;
            case SDL_EVENT_LOW_MEMORY:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
                }break;
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
                }break;
            case SDL_EVENT_DID_ENTER_BACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
                }break;
            case SDL_EVENT_WILL_ENTER_FOREGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
                }break;
            case SDL_EVENT_DID_ENTER_FOREGROUND:
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
        e.type = SDL_EVENT_QUIT;
        e.quit.timestamp = SDL_GetTicks();

        if( SDL_PushEvent( &e ) == false )
        {
            LOGGER_WARNING( "invalid push event [SDL_EVENT_QUIT] error: %s"
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
