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

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>
#import <sys/utsname.h>

#include <clocale>
#include <ctime>
#include <iomanip>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/sysctl.h>

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
SERVICE_FACTORY( PlatformService, Mengine::iOSPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSPlatformService::iOSPlatformService()
        : m_beginTime( 0 )
        , m_uiWindow( nil )
        , m_iOSInput( nullptr )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_desktop( false )
        , m_touchpad( false )
        , m_glContext( nil )
        , m_glView( nil )
        , m_mainScreenScale( 1.f )
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

        if( localeLanguage_str == nullptr )
        {
            return false;
        }

        size_t localeLanguageLen = StdString::strlen( localeLanguage_str );

        if( localeLanguageLen < 2 )
        {
            return false;
        }

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
        if( m_glView != nil )
        {
            GLint backingWidth = [m_glView backingWidth];
            GLint backingHeight = [m_glView backingHeight];

            if( backingWidth > 0 && backingHeight > 0 )
            {
                *_resolution = Resolution( (uint32_t)backingWidth, (uint32_t)backingHeight );

                return true;
            }
        }

        UIScreen * mainScreen = [UIScreen mainScreen];

        if( mainScreen == nil )
        {
            LOGGER_ERROR( "unable to access main screen" );
            
            return false;
        }

        CGRect screenRect = (m_uiWindow != nil) ? m_uiWindow.bounds : mainScreen.bounds;
        CGFloat screenScale = mainScreen.scale;

        *_resolution = Resolution( (uint32_t)(screenRect.size.width * screenScale), (uint32_t)(screenRect.size.height * screenScale) );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        UIDevice * device = [UIDevice currentDevice];

        LOGGER_INFO( "platform", "iOS platform: %s %s"
            , [[device systemName] UTF8String]
            , [[device systemVersion] UTF8String]
        );

        bool isTablet = (device.userInterfaceIdiom == UIUserInterfaceIdiomPad);

        LOGGER_INFO( "platform", "iOS Tablet: %s"
            , isTablet == true ? "true" : "false"
        );

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
        else if( HAS_OPTION( "ios" ) )
        {
            m_touchpad = true;
            m_desktop = false;

            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );
        }
        else
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

        iOSInputPtr iOSInput = Helper::makeFactorableUnique<Mengine::iOSInput>( MENGINE_DOCUMENT_FACTORABLE );

        if( iOSInput->initialize() == false )
        {
            return false;
        }

        m_iOSInput = iOSInput;
        
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

        LOGGER_INFO( "platform", "  Platform: iOS" );

        UIDevice * device = [UIDevice currentDevice];
        
        struct utsname systemInfo;
        uname( &systemInfo );

        LOGGER_INFO_PROTECTED( "platform", "  Device: %s (%s)"
            , [[device model] UTF8String]
            , systemInfo.machine
        );

        NSProcessInfo * processInfo = [NSProcessInfo processInfo];

        LOGGER_INFO_PROTECTED( "platform", "  CPU: %d cores"
            , (int)[processInfo processorCount]
        );

        LOGGER_INFO_PROTECTED( "platform", "  RAM: %dMB"
            , (int)([processInfo physicalMemory] / (1024 * 1024))
        );

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

        if( m_iOSInput != nullptr )
        {
            m_iOSInput->finalize();
            m_iOSInput = nullptr;
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

        if( m_glContext == nil || m_glView == nil )
        {
            return false;
        }

        if( [m_glView beginRender] == NO )
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

        if( [m_glView endRender] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::loopPlatform()
    {
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
    bool iOSPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically )
    {
        LOGGER_INFO( "platform", "open mail '%s' subject '%s' body '%s' technically '%s'"
            , _email
            , _subject
            , _body
            , _technically
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

        NSMutableString * mailBodyBuilder = [NSMutableString stringWithCapacity:4096];

        [mailBodyBuilder appendString:@"\n\n"];

        if( [@(_body) length] != 0 )
        {
            [mailBodyBuilder appendString:@(_body)];
            [mailBodyBuilder appendString:@"\n"];
        }

        [mailBodyBuilder appendString:@"----- Please Describe Your Message Above Here -----\n\n"];
        [mailBodyBuilder appendString:@"\n"];

        if( [@(_technically) length] != 0 )
        {
            [mailBodyBuilder appendString:@(_technically)];
            [mailBodyBuilder appendString:@"\n"];
        }

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

            NSDirectoryEnumerator<NSString *> * enumerator = [[NSFileManager defaultManager] enumeratorAtPath:userPathString];

            for( NSString * relativePath in enumerator )
            {
                NSString * fullPath = [userPathString stringByAppendingPathComponent:relativePath];

                BOOL isDirectory = NO;
                if( [[NSFileManager defaultManager] fileExistsAtPath:fullPath isDirectory:&isDirectory] == NO || isDirectory == YES )
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
            IOS_LOGGER_MESSAGE( @"open mail composer presented" );
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
            IOS_LOGGER_MESSAGE(@"delete account [YES]");
            
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
            IOS_LOGGER_MESSAGE(@"delete account [CANCEL]");
            
        }];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

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
        EAGLContext * glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if( glContext == nil )
        {
            LOGGER_ERROR( "invalid create EAGLContext for OpenGL ES 2.0" );

            return false;
        }

        if( [EAGLContext setCurrentContext:glContext] == NO )
        {
            LOGGER_ERROR( "invalid set current EAGLContext" );

            return false;
        }

        m_glContext = glContext;

        UIScreen * mainScreen = [UIScreen mainScreen];
        CGRect screenBounds = mainScreen.bounds;
        CGFloat screenScale = mainScreen.scale;

        UIViewController * rootViewController = m_uiWindow.rootViewController;

        iOSOpenGLView * glView = [[iOSOpenGLView alloc] initWithFrame:screenBounds context:m_glContext];

        if( glView == nil )
        {
            LOGGER_ERROR( "invalid create iOSOpenGLView" );

            return false;
        }

        glView.frame = m_uiWindow.bounds;
        glView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

        if( rootViewController != nil )
        {
            rootViewController.view = glView;
        }
        else
        {
            [m_uiWindow addSubview:glView];
        }

        [m_uiWindow makeKeyAndVisible];
        [m_uiWindow setNeedsLayout];
        [m_uiWindow layoutIfNeeded];
        [glView setNeedsLayout];
        [glView layoutIfNeeded];

        if( [glView backingWidth] == 0 || [glView backingHeight] == 0 )
        {
            if( [glView createFramebuffer] == NO )
            {
                LOGGER_ERROR( "invalid create framebuffer for iOSOpenGLView" );

                return false;
            }
        }

        m_glView = glView;

        int drawable_width = [glView backingWidth];
        int drawable_height = [glView backingHeight];

        LOGGER_INFO( "platform", "iOS drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );

        int win_width = (int)screenBounds.size.width;
        int win_height = (int)screenBounds.size.height;

        LOGGER_INFO( "platform", "iOS window size [%d, %d] scale [%.1f]"
            , win_width
            , win_height
            , (float)screenScale
        );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

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
        if( m_glView != nil )
        {
            GLint backingWidth = [m_glView backingWidth];
            GLint backingHeight = [m_glView backingHeight];

            if( backingWidth > 0 && backingHeight > 0 )
            {
                *_resolution = Resolution( (uint32_t)backingWidth, (uint32_t)backingHeight );

                return true;
            }
        }

        UIScreen * mainScreen = [UIScreen mainScreen];

        if( mainScreen == nil )
        {
            return false;
        }

        CGRect screenBounds = (m_uiWindow != nil) ? m_uiWindow.bounds : mainScreen.bounds;
        CGFloat screenScale = mainScreen.scale;

        uint32_t drawable_width = (uint32_t)(screenBounds.size.width * screenScale);
        uint32_t drawable_height = (uint32_t)(screenBounds.size.height * screenScale);

        *_resolution = Resolution( drawable_width, drawable_height );

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
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::minimizeWindow()
    {
        //Empty - not applicable on iOS
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        MENGINE_UNUSED( _pos );

        //Empty - not applicable on iOS
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
        //ToDo: implement native keyboard show
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::hideKeyboard()
    {
        //ToDo: implement native keyboard hide
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::isShowKeyboard() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fullscreen );

        //iOS is always fullscreen
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        //Empty - iOS uses CADisplayLink for vsync
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::notifyCursorModeChanged( bool _mode )
    {
        MENGINE_UNUSED( _mode );

        //Empty - no cursor on iOS
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

            return;
        }

        NSString * nsCaption = @(_caption);
        NSString * nsMessage = @(str);

        dispatch_async( dispatch_get_main_queue(), ^{
            [iOSDetail showOkAlertWithTitle:nsCaption
                                    message:nsMessage
                                         ok:^{}];
        });
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::setClipboardText( const Char * _value ) const
    {
        UIPasteboard * pasteboard = [UIPasteboard generalPasteboard];
        pasteboard.string = @(_value);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        UIPasteboard * pasteboard = [UIPasteboard generalPasteboard];
        NSString * text = pasteboard.string;

        if( text == nil )
        {
            return false;
        }

        StdString::strcpy_safe( _value, [text UTF8String], _capacity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleApplicationDidBecomeActive()
    {
        this->setActive_( true );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleApplicationWillEnterForeground()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleApplicationDidEnterBackground()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleApplicationWillResignActive()
    {
        this->setActive_( false );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleApplicationWillTerminate()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
    }
    //////////////////////////////////////////////////////////////////////////
    UIWindow * iOSPlatformService::getUIWindow() const
    {
        return m_uiWindow;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    //////////////////////////////////////////////////////////////////////////
    EAGLContext * iOSPlatformService::getEAGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view )
    {
        if( m_iOSInput != nullptr )
        {
            m_iOSInput->handleTouchBegan( _touches, _view );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view )
    {
        if( m_iOSInput != nullptr )
        {
            m_iOSInput->handleTouchMoved( _touches, _view );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view )
    {
        if( m_iOSInput != nullptr )
        {
            m_iOSInput->handleTouchEnded( _touches, _view );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view )
    {
        if( m_iOSInput != nullptr )
        {
            m_iOSInput->handleTouchCancelled( _touches, _view );
        }
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
        //Empty - iOS window setup is handled natively
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        UIWindow * existingWindow = nil;

        for( UIWindowScene * scene in [UIApplication sharedApplication].connectedScenes )
        {
            if( scene.activationState != UISceneActivationStateForegroundActive &&
                scene.activationState != UISceneActivationStateForegroundInactive )
            {
                continue;
            }

            for( UIWindow * window in scene.windows )
            {
                if( window.isKeyWindow == YES )
                {
                    existingWindow = window;
                    break;
                }
            }

            if( existingWindow == nil && [scene.windows count] > 0 )
            {
                existingWindow = [scene.windows firstObject];
            }

            if( existingWindow != nil )
            {
                break;
            }
        }

        if( existingWindow != nil )
        {
            m_uiWindow = existingWindow;
        }
        else
        {
            UIScreen * mainScreen = [UIScreen mainScreen];
            CGRect screenBounds = mainScreen.bounds;

            m_uiWindow = [[UIWindow alloc] initWithFrame:screenBounds];
        }

        if( m_uiWindow == nil )
        {
            LOGGER_ERROR( "create UIWindow failed" );

            return false;
        }

        CGRect windowBounds = m_uiWindow.frame;

        LOGGER_INFO( "platform", "iOS window created [%d, %d]"
            , (int)windowBounds.size.width
            , (int)windowBounds.size.height
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        if( m_glView != nil )
        {
            [m_glView destroyFramebuffer];
            [m_glView removeFromSuperview];
            m_glView = nil;
        }

        if( m_glContext != nil )
        {
            [EAGLContext setCurrentContext:nil];
            m_glContext = nil;
        }

        m_uiWindow = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSPlatformService::processEvents_()
    {
        //iOS manages its own event loop via UIApplication/UIKit run loop
        //Touch events are forwarded through iOSInput from the view controller

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSPlatformService::pushQuitEvent_()
    {
        APPLICATION_SERVICE()
            ->quit();
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
        m_iOSInput->getCursorPosition( &point );

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