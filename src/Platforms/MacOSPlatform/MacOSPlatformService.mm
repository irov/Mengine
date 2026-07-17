#import "MacOSPlatformService.h"

#import "MacOSWindowDelegate.h"

#import "Interface/LoggerInterface.h"
#import "Interface/FileServiceInterface.h"
#import "Interface/FileSystemInterface.h"
#import "Interface/ApplicationInterface.h"
#import "Interface/InputServiceInterface.h"
#import "Interface/RenderServiceInterface.h"
#import "Interface/LoggerServiceInterface.h"
#import "Interface/DateTimeSystemInterface.h"
#import "Interface/ThreadServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/MacOS/MacOSUtils.h"

#import "Kernel/FilePath.h"
#import "Kernel/PathHelper.h"
#import "Kernel/AssertionNotImplemented.h"
#import "Kernel/ConstStringHelper.h"
#import "Kernel/FilePathHelper.h"
#import "Kernel/FactorableUnique.h"
#import "Kernel/UnicodeHelper.h"
#import "Kernel/Logger.h"
#import "Kernel/ConfigurationHelper.h"
#import "Kernel/StringHelper.h"
#import "Kernel/BuildMode.h"
#import "Kernel/InputServiceHelper.h"
#import "Kernel/ConfigHelper.h"
#import "Kernel/OptionHelper.h"
#import "Kernel/TimestampHelper.h"
#import "Kernel/NotificationHelper.h"
#import "Kernel/VocabularyHelper.h"
#import "Kernel/EnumeratorHelper.h"

#import "Config/StdString.h"
#import "Config/StdIO.h"
#import "Config/StdAlgorithm.h"
#import "Config/Path.h"

#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import <CoreGraphics/CoreGraphics.h>
#import <sys/utsname.h>

#import "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include <clocale>
#include <ctime>
#include <unistd.h>

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
SERVICE_FACTORY( PlatformService, Mengine::MacOSPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSPlatformService::MacOSPlatformService()
        : m_beginTime( 0 )
        , m_window( nil )
        , m_windowDelegate( nil )
        , m_metalDevice( nil )
        , m_metalView( nil )
        , m_macOSWorkspace( nil )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_mainScreenScale( 1.f )
        , m_active( false )
        , m_desktop( true )
        , m_touchpad( false )
        , m_quitRequested( false )
        , m_cursorVisible( true )
        , m_cursorCaptureRequested( false )
        , m_cursorCaptureApplied( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSPlatformService::~MacOSPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSPlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        NSString * path = [[NSBundle mainBundle] resourcePath];

        if( path == nil )
        {
            path = [[NSBundle mainBundle] bundlePath];
        }

        const Char * pathRepresentation = [path fileSystemRepresentation];

        StdString::strcpy_safe( _currentPath, pathRepresentation, MENGINE_MAX_PATH );
        Helper::pathCorrectFolderPathA( _currentPath, MENGINE_PATH_FORWARDSLASH );
        Helper::pathCorrectBackslashA( _currentPath );

        size_t pathLen = StdString::strlen( _currentPath );

        return pathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSPlatformService::getUserPath( Char * const _userPath ) const
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
            LOGGER_ERROR( "error creating macOS user directory: %s"
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
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
    size_t MacOSPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
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
    bool MacOSPlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        NSString * localeLanguage = [[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode];

        if( localeLanguage == nil )
        {
            return false;
        }

        const Char * localeLanguageStr = [localeLanguage UTF8String];

        if( localeLanguageStr == nullptr )
        {
            return false;
        }

        size_t localeLanguageLen = StdString::strlen( localeLanguageStr );

        if( localeLanguageLen < 2 )
        {
            return false;
        }

        _userLocaleLanguage[0] = localeLanguageStr[0];
        _userLocaleLanguage[1] = localeLanguageStr[1];
        _userLocaleLanguage[2] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        return this->getDesktopResolution( _resolution );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        NSProcessInfo * processInfo = [NSProcessInfo processInfo];

        LOGGER_INFO( "platform", "macOS platform: %s"
            , [[processInfo operatingSystemVersionString] UTF8String]
        );

        m_platformTags.clear();
        m_desktop = true;
        m_touchpad = false;

        const Char * optionPlatforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t optionPlatformsCount;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "platform", optionPlatforms, &optionPlatformsCount ) == true )
        {
            for( uint32_t index = 0; index != optionPlatformsCount; ++index )
            {
                const Char * optionPlatform = optionPlatforms[index];

                m_platformTags.addTag( Helper::stringizeString( optionPlatform ) );
            }
        }
        else
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "MACOS" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "DESKTOP" ) );
        }

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        MacOSInputPtr input = Helper::makeFactorableUnique<MacOSInput>( MENGINE_DOCUMENT_FACTORABLE );

        if( input->initialize() == false )
        {
            return false;
        }

        m_input = input;

        NSScreen * mainScreen = [NSScreen mainScreen];
        if( mainScreen != nil )
        {
            m_mainScreenScale = (float)[mainScreen backingScaleFactor];
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::_runService()
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
        LOGGER_INFO( "platform", "  Platform: macOS" );

        struct utsname systemInfo;
        uname( &systemInfo );

        LOGGER_INFO_PROTECTED( "platform", "  Device: %s"
            , systemInfo.machine
        );

        NSProcessInfo * processInfo = [NSProcessInfo processInfo];

        LOGGER_INFO_PROTECTED( "platform", "  CPU: %d cores"
            , (int)[processInfo processorCount]
        );

        LOGGER_INFO_PROTECTED( "platform", "  RAM: %dMB"
            , (int)([processInfo physicalMemory] / (1024 * 1024))
        );

        m_macOSWorkspace = [[MacOSWorkspace alloc] init];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::_stopService()
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
    void MacOSPlatformService::_finalizeService()
    {
        m_cursorCaptureRequested = false;
        this->updateCursorCapture_();

        m_active = false;

        this->destroyWindow_();

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_DOCUMENT_ENABLE)
        for( const MacOSEventHandlerDesc & desc : m_eventHandlers )
        {
            MENGINE_UNUSED( desc );

            LOGGER_ASSERTION( "forgot remove macOS event handler (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#   else
        MENGINE_ASSERTION_FATAL( m_eventHandlers.empty() == true, "forgot remove macOS event handlers" );
#   endif
#endif

        m_eventHandlers.clear();

        if( m_input != nullptr )
        {
            m_input->finalize();
            m_input = nullptr;
        }

        m_macOSWorkspace = nil;
        m_platformTags.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::runPlatform()
    {
        Timestamp timestamp = this->getInputTimestamp();

        this->setActive_( true, timestamp );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::tickPlatform( float _frameTime )
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

            THREAD_SYSTEM()
                ->sleep( 1 );
        }
        else
        {
            bool OPTION_maxfps = HAS_OPTION( "maxfps" );

            if( APPLICATION_SERVICE()
                ->getVSync() == false && OPTION_maxfps == false )
            {
                THREAD_SYSTEM()
                    ->sleep( 1 );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::renderPlatform()
    {
        if( this->isNeedWindowRender() == false )
        {
            return false;
        }

        if( m_metalView == nil )
        {
            return false;
        }

        if( [m_metalView beginRender] == NO )
        {
            return false;
        }

        id<CAMetalDrawable> drawable = [m_metalView currentDrawable];

        if( drawable == nil )
        {
            [m_metalView endRender];

            return false;
        }

        MetalRenderSystemExtensionInterface * renderSystemExtension = RENDER_SYSTEM()
            ->getUnknown();

        renderSystemExtension->setCurrentDrawable( drawable, drawable.texture );

        bool successful = APPLICATION_SERVICE()
            ->render();

        if( successful == false )
        {
            [m_metalView endRender];

            return false;
        }

        APPLICATION_SERVICE()
            ->flush();

        [m_metalView endRender];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::loopPlatform()
    {
        m_prevTime = Helper::getElapsedTime();

        for( ;; )
        {
            @autoreleasepool
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
                    THREAD_SYSTEM()
                        ->sleep( 100 );

                    continue;
                }

                this->tickPlatform( frameTime );

                if( this->renderPlatform() == false )
                {
                    THREAD_SYSTEM()
                        ->sleep( 100 );

                    continue;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        NSURL * url = [NSURL URLWithString:@(_url)];

        if( url == nil )
        {
            return false;
        }

        BOOL result = [[NSWorkspace sharedWorkspace] openURL:url];

        return result == YES;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically )
    {
        MENGINE_UNUSED( _technically );

        NSString * mail = [NSString stringWithFormat:@"mailto:%s?subject=%@&body=%@"
            , _email
            , [@(_subject) stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]
            , [@(_body) stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]
        ];

        return this->openUrlInDefaultBrowser( [mail UTF8String] );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::openDeleteAccount()
    {
        LOGGER_INFO( "platform", "open delete account" );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp MacOSPlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp MacOSPlatformService::getInputTimestamp() const
    {
        Timestamp timestamp = Helper::getPlatformTimestamp();

        return timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        m_projectTitle.assign( _projectTitle );

        if( m_window != nil )
        {
            [m_window setTitle:@(_projectTitle)];
        }

        LOGGER_INFO( "platform", "project title: %s"
            , m_projectTitle.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MacOSPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::alreadyRunningMonitor()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
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
    bool MacOSPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & MacOSPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MacOSPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return static_cast<int32_t>(_dp * m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MacOSPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        return static_cast<int32_t>(_dp * m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MacOSPlatformService::pxToWidthDp( int32_t _px ) const
    {
        return static_cast<int32_t>(_px / m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MacOSPlatformService::pxToHeightDp( int32_t _px ) const
    {
        return static_cast<int32_t>(_px / m_mainScreenScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr MacOSPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        LOGGER_ERROR( "macOS native dynamic library loading is not implemented: '%s'"
            , _dynamicLibraryName
        );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        if( m_metalView != nil )
        {
            NSInteger drawableWidth = [m_metalView drawableWidth];
            NSInteger drawableHeight = [m_metalView drawableHeight];

            if( drawableWidth > 0 && drawableHeight > 0 )
            {
                *_resolution = Resolution( (uint32_t)drawableWidth, (uint32_t)drawableHeight );

                return true;
            }
        }

        NSScreen * screen = m_window != nil ? [m_window screen] : [NSScreen mainScreen];

        if( screen == nil )
        {
            return false;
        }

        NSRect frame = [screen frame];
        CGFloat scale = [screen backingScaleFactor];

        *_resolution = Resolution( (uint32_t)(frame.size.width * scale), (uint32_t)(frame.size.height * scale) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::isDebuggerPresent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::debugBreak()
    {
        __builtin_trap();
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::closeWindow()
    {
        m_quitRequested = true;

        if( m_window != nil )
        {
            [m_window orderOut:nil];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::minimizeWindow()
    {
        if( m_window != nil )
        {
            [m_window miniaturize:nil];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        NSView * view = this->getNSView();

        if( m_window == nil || view == nil )
        {
            return;
        }

        NSRect bounds = [view bounds];
        NSPoint viewPoint = NSMakePoint( _pos.x * bounds.size.width, (1.f - _pos.y) * bounds.size.height );
        NSPoint windowPoint = [view convertPoint:viewPoint toView:nil];
        NSRect screenRect = [m_window convertRectToScreen:NSMakeRect( windowPoint.x, windowPoint.y, 1.0, 1.0 )];

        NSScreen * screen = [m_window screen];
        CGFloat screenHeight = screen != nil ? [screen frame].size.height : [[NSScreen mainScreen] frame].size.height;

        CGPoint cgPoint = CGPointMake( screenRect.origin.x, screenHeight - screenRect.origin.y );

        CGWarpMouseCursorPosition( cgPoint );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::setCursorCapture( bool _capture )
    {
        m_cursorCaptureRequested = _capture;

        return this->updateCursorCapture_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::showKeyboard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::hideKeyboard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::isShowKeyboard() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_window == nil )
        {
            return true;
        }

        NSWindowStyleMask styleMask = [m_window styleMask];
        bool alreadyFullscreen = (styleMask & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen;

        if( alreadyFullscreen != _fullscreen )
        {
            if( this->changeWindow_( _resolution, _fullscreen ) == false )
            {
                return false;
            }
        }
        else if( _fullscreen == false )
        {
            if( m_metalView != nil )
            {
                NSInteger drawableWidth = [m_metalView drawableWidth];
                NSInteger drawableHeight = [m_metalView drawableHeight];

                if( drawableWidth == (NSInteger)_resolution.getWidth() && drawableHeight == (NSInteger)_resolution.getHeight() )
                {
                    return true;
                }
            }

            CGFloat width = _resolution.getWidthF();
            CGFloat height = _resolution.getHeightF();
            CGFloat scale = [m_window backingScaleFactor];

            if( scale > 0.0 )
            {
                width /= scale;
                height /= scale;
            }

            NSSize contentSize = NSMakeSize( width, height );
            [m_window setContentSize:contentSize];
            this->updateWindowResolution_();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::notifyVsyncChanged( bool _vsync )
    {
        if( m_metalView != nil )
        {
            [m_metalView updateVSync:_vsync == true ? YES : NO];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::notifyCursorModeChanged( bool _mode )
    {
        if( m_cursorVisible == _mode )
        {
            return;
        }

        m_cursorVisible = _mode;

        if( _mode == true )
        {
            [NSCursor unhide];
        }
        else
        {
            [NSCursor hide];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        Path pathPictures = {'\0'};
        if( Helper::MacOSGetPicturesDirectory( pathPictures ) == false )
        {
            LOGGER_ERROR( "invalid get pictures directory" );

            return false;
        }

        Path pathFile = {'\0'};
        MENGINE_SNPRINTF( pathFile, MENGINE_MAX_PATH, "%s%s%s", pathPictures, _directoryPath, _filePath );

        if( Helper::MacOSSetDesktopWallpaper( pathFile ) == false )
        {
            LOGGER_ERROR( "error set desktop wallpaper '%s'"
                , pathFile
            );

            return false;
        }

        if( m_window != nil && ([m_window styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen )
        {
            [m_macOSWorkspace changeDesktopWallpaper:pathFile];
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        Path pathPictures = {'\0'};
        if( Helper::MacOSGetPicturesDirectory( pathPictures ) == false )
        {
            LOGGER_ERROR( "invalid get pictures directory" );

            return false;
        }

        return this->writeUserDirectoryFile_( pathPictures, _directoryPath, _filePath, _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        Path pathMusic = {'\0'};
        if( Helper::MacOSGetMusicDirectory( pathMusic ) == false )
        {
            LOGGER_ERROR( "invalid get music directory" );

            return false;
        }

        return this->writeUserDirectoryFile_( pathMusic, _directoryPath, _filePath, _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::writeUserDirectoryFile_( const Char * _basePath, const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) const
    {
        if( FILE_SYSTEM()
            ->createDirectory( _basePath, _directoryPath ) == false )
        {
            LOGGER_ERROR( "invalid create directory '%s%s'"
                , _basePath
                , _directoryPath
            );

            return false;
        }

        Path pathFile = {'\0'};
        MENGINE_SNPRINTF( pathFile, MENGINE_MAX_PATH, "%s%s%s", _basePath, _directoryPath, _filePath );

        NSData * data = [NSData dataWithBytes:_data length:_size];
        NSString * filePath = @(pathFile);

        NSError * error = nil;
        if( [data writeToFile:filePath options:NSDataWritingAtomic error:&error] == NO )
        {
            LOGGER_ERROR( "invalid write file '%s' error '%s'"
                , pathFile
                , error != nil ? [[AppleDetail getMessageFromNSError:error] UTF8String] : "unknown"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t sizeVsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        if( sizeVsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            return;
        }

        if( Helper::isSilentDialog() == true )
        {
            LOGGER_MESSAGE( "[messageBox] %s: %s"
                , _caption
                , str
            );

            return;
        }

        Helper::MacOSShowAlert( _caption, str );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::setClipboardText( const Char * _value ) const
    {
        NSPasteboard * pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard clearContents];
        BOOL result = [pasteboard setString:@(_value) forType:NSPasteboardTypeString];

        return result == YES;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        NSPasteboard * pasteboard = [NSPasteboard generalPasteboard];
        NSString * text = [pasteboard stringForType:NSPasteboardTypeString];

        if( text == nil )
        {
            return false;
        }

        StdString::strcpy_safe( _value, [text UTF8String], _capacity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleApplicationDidBecomeActive()
    {
        this->setActive_( true, this->getInputTimestamp() );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleApplicationWillResignActive()
    {
        this->setActive_( false, this->getInputTimestamp() );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleApplicationWillTerminate()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowDidResize()
    {
        this->updateWindowResolution_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowDidBecomeKey()
    {
        this->setActive_( true, this->getInputTimestamp() );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowDidResignKey()
    {
        this->setActive_( false, this->getInputTimestamp() );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowDidMiniaturize()
    {
        this->setActive_( false, this->getInputTimestamp() );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowDidDeminiaturize()
    {
        this->setActive_( true, this->getInputTimestamp() );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::handleWindowWillClose()
    {
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    NSWindow * MacOSPlatformService::getNSWindow() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    NSView * MacOSPlatformService::getNSView() const
    {
        return m_metalView;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId MacOSPlatformService::addMacOSEventHandler( const LambdaMacOSEventHandler & _lambda, const DocumentInterfacePtr & _doc )
    {
        UniqueId id = Helper::generateUniqueIdentity();

        MacOSEventHandlerDesc desc;
        desc.id = id;
        desc.lambda = _lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#else
        MENGINE_UNUSED( _doc );
#endif

        m_eventHandlers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::removeMacOSEventHandler( UniqueId _id )
    {
        VectorMacOSEventHandlers::iterator it_found = StdAlgorithm::find_if( m_eventHandlers.begin(), m_eventHandlers.end(), [_id]( const MacOSEventHandlerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_eventHandlers.end(), "not found macOS event handler '%u'"
            , _id
        );

        m_eventHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLDevice> MacOSPlatformService::getMetalDevice() const
    {
        return m_metalDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

        RENDER_SYSTEM()
            ->onWindowChangeFullscreenPrepare( _fullscreen );

        if( m_window != nil )
        {
            [m_window toggleFullScreen:nil];
        }

        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::setupWindow_()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        NSScreen * screen = [NSScreen mainScreen];

        if( screen == nil )
        {
            LOGGER_ERROR( "invalid get main screen" );

            return false;
        }

        NSRect screenFrame = [screen frame];
        CGFloat scale = [screen backingScaleFactor];

        NSRect contentRect;

        if( _fullscreen == true )
        {
            contentRect = screenFrame;
        }
        else
        {
            CGFloat width = _windowResolution.getWidthF();
            CGFloat height = _windowResolution.getHeightF();

            if( scale > 0.f )
            {
                width /= scale;
                height /= scale;
            }

            contentRect = NSMakeRect(
                screenFrame.origin.x + (screenFrame.size.width - width) * 0.5,
                screenFrame.origin.y + (screenFrame.size.height - height) * 0.5,
                width,
                height
            );
        }

        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled |
            NSWindowStyleMaskClosable |
            NSWindowStyleMaskMiniaturizable |
            NSWindowStyleMaskResizable;

        NSWindow * window = [[NSWindow alloc] initWithContentRect:contentRect
                                                        styleMask:styleMask
                                                          backing:NSBackingStoreBuffered
                                                            defer:NO
                                                           screen:screen];

        if( window == nil )
        {
            LOGGER_ERROR( "invalid create NSWindow" );

            return false;
        }

        [window setTitle:@(m_projectTitle.c_str())];
        [window setAcceptsMouseMovedEvents:YES];
        [window setReleasedWhenClosed:NO];
        [window setRestorable:NO];

        MacOSWindowDelegate * windowDelegate = [[MacOSWindowDelegate alloc] initWithPlatformService:this];
        [window setDelegate:windowDelegate];

        m_window = window;
        m_windowDelegate = windowDelegate;

        if( _fullscreen == true )
        {
            [window toggleFullScreen:nil];
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::applyWindow_()
    {
        if( m_window == nil )
        {
            return false;
        }

        id<MTLDevice> metalDevice = MTLCreateSystemDefaultDevice();

        if( metalDevice == nil )
        {
            LOGGER_ERROR( "invalid create MTLDevice" );

            return false;
        }

        m_metalDevice = metalDevice;

        NSView * contentView = [m_window contentView];
        NSRect contentBounds = contentView != nil ? [contentView bounds] : [m_window frame];

        MacOSMetalView * metalView = [[MacOSMetalView alloc] initWithFrame:contentBounds device:m_metalDevice];

        if( metalView == nil )
        {
            LOGGER_ERROR( "invalid create MacOSMetalView" );

            return false;
        }

        [metalView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        [m_window setContentView:metalView];
        [m_window makeFirstResponder:metalView];
        [m_window makeKeyAndOrderFront:nil];

        m_metalView = metalView;

        [m_metalView updateVSync:APPLICATION_SERVICE()->getVSync() == true ? YES : NO];

        LOGGER_INFO( "platform", "macOS Metal device: %s"
            , [[m_metalDevice name] UTF8String]
        );

        LOGGER_INFO( "platform", "macOS Metal drawable size [%ld, %ld]"
            , (long)[m_metalView drawableWidth]
            , (long)[m_metalView drawableHeight]
        );

        LOGGER_INFO( "platform", "macOS window size [%d, %d] scale [%.1f]"
            , (int)contentBounds.size.width
            , (int)contentBounds.size.height
            , m_mainScreenScale
        );

        this->updateWindowResolution_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        if( m_metalView != nil )
        {
            [m_metalView removeFromSuperview];
            m_metalView = nil;
        }

        m_metalDevice = nil;

        if( m_window != nil )
        {
            [m_window setDelegate:nil];
            [m_window close];
            m_window = nil;
        }

        m_windowDelegate = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::processEvents_()
    {
        @autoreleasepool
        {
            for( ;; )
            {
                NSEvent * event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                     untilDate:[NSDate distantPast]
                                                        inMode:NSDefaultRunLoopMode
                                                       dequeue:YES];

                if( event == nil )
                {
                    break;
                }

                NSView * view = this->getNSView();
                bool handled = false;

                for( const MacOSEventHandlerDesc & desc : m_eventHandlers )
                {
                    if( desc.lambda( view, event ) == true )
                    {
                        handled = true;
                        break;
                    }
                }

                if( handled == false && m_input != nullptr )
                {
                    m_input->handleEvent( view, event );
                }

                if( handled == false && [event type] == NSEventTypeKeyDown )
                {
                    unsigned short keyCode = [event keyCode];
                    NSEventModifierFlags modifierFlags = [event modifierFlags];

                    if( keyCode == kVK_Return && (modifierFlags & NSEventModifierFlagOption) != 0 )
                    {
                        if( APPLICATION_SERVICE()
                            ->getAllowFullscreenSwitchShortcut() == true )
                        {
                            bool fullscreen = APPLICATION_SERVICE()
                                ->getFullscreenMode();

                            APPLICATION_SERVICE()
                                ->setFullscreenMode( !fullscreen );
                        }
                    }
                }

                [NSApp sendEvent:event];
            }

            [NSApp updateWindows];
        }

        return m_quitRequested;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::pushQuitEvent_()
    {
        if( m_quitRequested == true )
        {
            return;
        }

        m_quitRequested = true;

        if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::ApplicationInterface ) == true )
        {
            APPLICATION_SERVICE()
                ->quit();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSPlatformService::setActive_( bool _active, Timestamp _timestamp )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        this->updateCursorCapture_();

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        mt::vec2f point;
        if( m_input != nullptr )
        {
            m_input->getCursorPosition( this->getNSView(), &point );
        }
        else
        {
            point = mt::vec2f( 0.f, 0.f );
        }

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( _timestamp, TC_TOUCH0, point.x, point.y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( _timestamp, TC_TOUCH0, point.x, point.y, 0.f );
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
    bool MacOSPlatformService::updateCursorCapture_()
    {
        const bool capture = m_cursorCaptureRequested == true && m_active == true && m_window != nil;

        if( m_cursorCaptureApplied == capture )
        {
            return true;
        }

        if( capture == true )
        {
            this->setCursorPosition( mt::vec2f( 0.5f, 0.5f ) );
        }

        CGError result = CGAssociateMouseAndMouseCursorPosition( capture == true ? false : true );

        if( result != kCGErrorSuccess )
        {
            LOGGER_ERROR( "invalid set macOS cursor capture [%u] error [%d]"
                , capture
                , result
            );

            return false;
        }

        m_cursorCaptureApplied = capture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::isNeedWindowRender() const
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
    void MacOSPlatformService::updateWindowResolution_()
    {
        Resolution resolution;
        if( this->getDesktopResolution( &resolution ) == false )
        {
            return;
        }

        APPLICATION_SERVICE()
            ->setWindowResolution( resolution );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSPlatformService::initializeFileService()
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
    void MacOSPlatformService::finalizeFileService()
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
