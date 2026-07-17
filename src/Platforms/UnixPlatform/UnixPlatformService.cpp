#include "UnixPlatformService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "UnixDynamicLibrary.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConfigurationHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/Path.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include <GL/glxext.h>

#include <cerrno>
#include <climits>
#include <clocale>
#include <csignal>
#include <cstdlib>
#include <cstring>

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
#   define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef GLX_CONTEXT_MINOR_VERSION_ARB
#   define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef GLX_CONTEXT_PROFILE_MASK_ARB
#   define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#endif

#ifndef GLX_CONTEXT_CORE_PROFILE_BIT_ARB
#   define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif

#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#   define MENGINE_DEVELOPMENT_USER_FOLDER_NAME "User"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::UnixPlatformService );
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        using GLXCreateContextAttribsProc = GLXContext (*)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
        using GLXSwapIntervalEXTProc = void (*)(Display *, GLXDrawable, int);
        using GLXSwapIntervalMESAProc = int (*)(unsigned int);
        using GLXSwapIntervalSGIProc = int (*)(int);

        struct KeyRepeatCheckData
        {
            const XEvent * event;
            bool found;
        };

        static bool s_contextError = false;
        //////////////////////////////////////////////////////////////////////////
        static int contextErrorHandler( Display * _display, XErrorEvent * _event )
        {
            MENGINE_UNUSED( _display );
            MENGINE_UNUSED( _event );

            s_contextError = true;

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool ensureDirectory( const Char * _path )
        {
            if( ::mkdir( _path, S_IRWXU ) == 0 )
            {
                return true;
            }

            if( errno != EEXIST )
            {
                return false;
            }

            struct stat existing;

            if( ::stat( _path, &existing ) != 0 )
            {
                return false;
            }

            if( S_ISDIR( existing.st_mode ) == 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool hasExtension( const Char * _extensions, const Char * _extension )
        {
            if( _extensions == nullptr || _extension == nullptr || _extension[0] == '\0' || StdString::strchr( _extension, ' ' ) != nullptr )
            {
                return false;
            }

            const size_t extensionLength = StdString::strlen( _extension );
            const Char * cursor = _extensions;

            for( ;; )
            {
                const Char * found = StdString::strstr( cursor, _extension );

                if( found == nullptr )
                {
                    return false;
                }

                const bool beginValid = found == _extensions || found[-1] == ' ';
                const Char end = found[extensionLength];
                const bool endValid = end == '\0' || end == ' ';

                if( beginValid == true && endValid == true )
                {
                    return true;
                }

                cursor = found + extensionLength;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static Bool keyRepeatCheck( Display * _display, XEvent * _event, XPointer _data )
        {
            MENGINE_UNUSED( _display );

            KeyRepeatCheckData * data = reinterpret_cast<KeyRepeatCheckData *>(_data);

            if( _event->type == KeyPress &&
                _event->xkey.keycode == data->event->xkey.keycode &&
                _event->xkey.time - data->event->xkey.time < 2 )
            {
                data->found = true;
            }

            return False;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isKeyRepeat( Display * _display, const XEvent & _event )
        {
            if( ::XPending( _display ) == 0 )
            {
                return false;
            }

            KeyRepeatCheckData data;
            data.event = &_event;
            data.found = false;

            XEvent dummyEvent;
            ::XCheckIfEvent( _display, &dummyEvent, &Detail::keyRepeatCheck, reinterpret_cast<XPointer>(&data) );

            return data.found;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool createDirectories( const Char * _path )
        {
            Path path = {'\0'};
            StdString::strcpy_safe( path, _path, MENGINE_MAX_PATH );

            size_t length = StdString::strlen( path );

            if( length == 0 )
            {
                return false;
            }

            for( Char * cursor = path + 1; *cursor != '\0'; ++cursor )
            {
                if( *cursor != '/' )
                {
                    continue;
                }

                *cursor = '\0';

                if( Detail::ensureDirectory( path ) == false )
                {
                    return false;
                }

                *cursor = '/';
            }

            if( path[length - 1] != '/' )
            {
                if( Detail::ensureDirectory( path ) == false )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool launchDetached( const Char * _executable, const Char * const * _arguments )
        {
            int errorPipe[2];

            if( ::pipe( errorPipe ) != 0 )
            {
                return false;
            }

            if( ::fcntl( errorPipe[0], F_SETFD, FD_CLOEXEC ) == -1 ||
                ::fcntl( errorPipe[1], F_SETFD, FD_CLOEXEC ) == -1 )
            {
                ::close( errorPipe[0] );
                ::close( errorPipe[1] );

                return false;
            }

            pid_t child = ::fork();

            if( child < 0 )
            {
                ::close( errorPipe[0] );
                ::close( errorPipe[1] );

                return false;
            }

            if( child == 0 )
            {
                ::close( errorPipe[0] );

                pid_t grandchild = ::fork();

                if( grandchild < 0 )
                {
                    const int forkError = errno;
                    MENGINE_UNUSED( ::write( errorPipe[1], &forkError, sizeof( forkError ) ) );

                    ::_exit( EXIT_FAILURE );
                }

                if( grandchild > 0 )
                {
                    ::close( errorPipe[1] );

                    ::_exit( EXIT_SUCCESS );
                }

                if( ::setsid() < 0 )
                {
                    const int sessionError = errno;
                    MENGINE_UNUSED( ::write( errorPipe[1], &sessionError, sizeof( sessionError ) ) );

                    ::_exit( EXIT_FAILURE );
                }

                int nullFd = ::open( "/dev/null", O_RDWR );
                if( nullFd >= 0 )
                {
                    ::dup2( nullFd, STDIN_FILENO );
                    ::dup2( nullFd, STDOUT_FILENO );
                    ::dup2( nullFd, STDERR_FILENO );

                    if( nullFd > STDERR_FILENO )
                    {
                        ::close( nullFd );
                    }
                }

                ::execvp( _executable, const_cast<Char * const *>(_arguments) );

                const int executeError = errno;
                MENGINE_UNUSED( ::write( errorPipe[1], &executeError, sizeof( executeError ) ) );

                ::_exit( EXIT_FAILURE );
            }

            ::close( errorPipe[1] );

            int status = 0;

            while( ::waitpid( child, &status, 0 ) < 0 )
            {
                if( errno != EINTR )
                {
                    ::close( errorPipe[0] );

                    return false;
                }
            }

            if( WIFEXITED( status ) == 0 || WEXITSTATUS( status ) != EXIT_SUCCESS )
            {
                ::close( errorPipe[0] );

                return false;
            }

            int executeError = 0;
            size_t errorSize = 0;

            while( errorSize != sizeof( executeError ) )
            {
                ssize_t readSize = ::read( errorPipe[0], reinterpret_cast<Char *>(&executeError) + errorSize, sizeof( executeError ) - errorSize );

                if( readSize > 0 )
                {
                    errorSize += (size_t)readSize;

                    continue;
                }

                if( readSize == 0 )
                {
                    break;
                }

                if( errno == EINTR )
                {
                    continue;
                }

                ::close( errorPipe[0] );

                return false;
            }

            ::close( errorPipe[0] );

            if( errorSize != 0 )
            {
                LOGGER_ERROR( "failed to execute '%s': %s"
                    , _executable
                    , ::strerror( executeError )
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void appendUrlEncoded( String * const _uri, const Char * _value )
        {
            static const Char hex[] = "0123456789ABCDEF";

            for( const unsigned char * cursor = reinterpret_cast<const unsigned char *>(_value); *cursor != '\0'; ++cursor )
            {
                const unsigned char ch = *cursor;

                if( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '~' )
                {
                    _uri->push_back( (Char)ch );
                }
                else
                {
                    _uri->push_back( '%' );
                    _uri->push_back( hex[(ch >> 4) & 0x0F] );
                    _uri->push_back( hex[ch & 0x0F] );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    UnixPlatformService::UnixPlatformService()
        : m_beginTime( 0 )
        , m_display( nullptr )
        , m_screen( 0 )
        , m_window( 0 )
        , m_clipboardWindow( 0 )
        , m_colormap( 0 )
        , m_glContext( nullptr )
        , m_inputMethod( nullptr )
        , m_inputContext( nullptr )
        , m_wmProtocolsAtom( 0 )
        , m_wmDeleteWindowAtom( 0 )
        , m_netWmStateAtom( 0 )
        , m_netWmStateFullscreenAtom( 0 )
        , m_netWorkAreaAtom( 0 )
        , m_clipboardAtom( 0 )
        , m_targetsAtom( 0 )
        , m_utf8StringAtom( 0 )
        , m_textPlainAtom( 0 )
        , m_clipboardPropertyAtom( 0 )
        , m_defaultCursor( 0 )
        , m_hiddenCursor( 0 )
        , m_prevTime( 0.0 )
        , m_pauseUpdatingTime( -1.f )
        , m_displayScale( 1.f )
        , m_active( false )
        , m_fullscreen( false )
        , m_quitRequested( false )
        , m_windowAttached( false )
        , m_windowExposed( false )
        , m_cursorVisible( true )
        , m_keyboardVisible( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UnixPlatformService::~UnixPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

        ::setlocale( LC_ALL, "" );
        ::XSetLocaleModifiers( "" );

        m_display = ::XOpenDisplay( nullptr );

        if( m_display == nullptr )
        {
            LOGGER_ERROR( "XOpenDisplay failed; check DISPLAY" );

            return false;
        }

        m_screen = DefaultScreen( m_display );

        m_wmProtocolsAtom = ::XInternAtom( m_display, "WM_PROTOCOLS", False );
        m_wmDeleteWindowAtom = ::XInternAtom( m_display, "WM_DELETE_WINDOW", False );
        m_netWmStateAtom = ::XInternAtom( m_display, "_NET_WM_STATE", False );
        m_netWmStateFullscreenAtom = ::XInternAtom( m_display, "_NET_WM_STATE_FULLSCREEN", False );
        m_netWorkAreaAtom = ::XInternAtom( m_display, "_NET_WORKAREA", False );
        m_clipboardAtom = ::XInternAtom( m_display, "CLIPBOARD", False );
        m_targetsAtom = ::XInternAtom( m_display, "TARGETS", False );
        m_utf8StringAtom = ::XInternAtom( m_display, "UTF8_STRING", False );
        m_textPlainAtom = ::XInternAtom( m_display, "text/plain;charset=utf-8", False );
        m_clipboardPropertyAtom = ::XInternAtom( m_display, "MENGINE_CLIPBOARD", False );

        Window root = RootWindow( m_display, m_screen );
        XSetWindowAttributes clipboardAttributes;
        StdString::memset( &clipboardAttributes, 0, sizeof( clipboardAttributes ) );
        clipboardAttributes.event_mask = PropertyChangeMask;

        m_clipboardWindow = ::XCreateWindow( m_display, root, -10, -10, 1, 1, 0, CopyFromParent, InputOnly, CopyFromParent, CWEventMask, &clipboardAttributes );

        m_inputMethod = ::XOpenIM( m_display, nullptr, nullptr, nullptr );

        m_defaultCursor = ::XCreateFontCursor( m_display, XC_left_ptr );

        static const Char emptyBitmap[1] = {0};
        Pixmap emptyPixmap = ::XCreateBitmapFromData( m_display, root, emptyBitmap, 1, 1 );

        if( emptyPixmap != 0 )
        {
            XColor emptyColor;
            StdString::memset( &emptyColor, 0, sizeof( emptyColor ) );
            m_hiddenCursor = ::XCreatePixmapCursor( m_display, emptyPixmap, emptyPixmap, &emptyColor, &emptyColor, 0, 0 );
            ::XFreePixmap( m_display, emptyPixmap );
        }

        const int displayWidth = DisplayWidth( m_display, m_screen );
        const int displayWidthMM = DisplayWidthMM( m_display, m_screen );

        if( displayWidth > 0 && displayWidthMM > 0 )
        {
            const float dpi = (float)displayWidth * 25.4f / (float)displayWidthMM;
            m_displayScale = dpi / 96.f;

            if( m_displayScale < 1.f )
            {
                m_displayScale = 1.f;
            }
            else if( m_displayScale > 4.f )
            {
                m_displayScale = 4.f;
            }
        }

        m_platformTags.clear();

        const Char * optionPlatforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t optionPlatformsCount = 0;
        if( OPTIONS_SERVICE()->getOptionValues( "platform", optionPlatforms, &optionPlatformsCount ) == true )
        {
            for( uint32_t index = 0; index != optionPlatformsCount; ++index )
            {
                m_platformTags.addTag( Helper::stringizeString( optionPlatforms[index] ) );
            }
        }
        else
        {
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "LINUX" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "UNIX" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "DESKTOP" ) );
        }

        m_factoryDynamicLibraries = Helper::makeFactoryPool<UnixDynamicLibrary, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::_finalizeService()
    {
        m_active = false;

        this->destroyWindow_();

#if defined(MENGINE_DEBUG)
#   if defined(MENGINE_DOCUMENT_ENABLE)
        for( const UnixEventHandlerDesc & desc : m_eventHandlers )
        {
            LOGGER_ASSERTION( "forgot to remove Unix event handler (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#   else
        MENGINE_ASSERTION_FATAL( m_eventHandlers.empty() == true, "forgot to remove Unix event handlers" );
#   endif
#endif

        m_eventHandlers.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );
        m_factoryDynamicLibraries = nullptr;

        if( m_inputMethod != nullptr )
        {
            ::XCloseIM( m_inputMethod );
            m_inputMethod = nullptr;
        }

        if( m_defaultCursor != 0 )
        {
            ::XFreeCursor( m_display, m_defaultCursor );
            m_defaultCursor = 0;
        }

        if( m_hiddenCursor != 0 )
        {
            ::XFreeCursor( m_display, m_hiddenCursor );
            m_hiddenCursor = 0;
        }

        if( m_clipboardWindow != 0 )
        {
            ::XDestroyWindow( m_display, m_clipboardWindow );
            m_clipboardWindow = 0;
        }

        if( m_display != nullptr )
        {
            ::XCloseDisplay( m_display );
            m_display = nullptr;
        }

        m_platformTags.clear();
        m_clipboardText.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::_runService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "start date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        struct utsname systemInfo;
        if( ::uname( &systemInfo ) == 0 )
        {
            LOGGER_INFO( "platform", "Linux %s %s (%s)"
                , systemInfo.release
                , systemInfo.version
                , systemInfo.machine
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::_stopService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()->getLocalDateTime( &dateTime );

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
    bool UnixPlatformService::runPlatform()
    {
        this->setActive_( true, this->getInputTimestamp() );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        this->tickPlatform( 0.f );

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::loopPlatform()
    {
        m_prevTime = Helper::getElapsedTime();

        for( ;; )
        {
            if( this->updatePlatform() == false )
            {
                break;
            }

            const double currentTime = Helper::getElapsedTime();
            const float frameTime = (float)(currentTime - m_prevTime);
            m_prevTime = currentTime;

            if( m_active == false )
            {
                THREAD_SYSTEM()->sleep( 100 );

                continue;
            }

            this->tickPlatform( frameTime );

            if( this->renderPlatform() == false )
            {
                THREAD_SYSTEM()->sleep( 10 );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::updatePlatform()
    {
        if( this->processEvents_() == false )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::tickPlatform( float _frameTime )
    {
        bool updating = APPLICATION_SERVICE()->beginUpdate();

        if( updating == true )
        {
            if( m_pauseUpdatingTime >= 0.f )
            {
                _frameTime = m_pauseUpdatingTime;
                m_pauseUpdatingTime = -1.f;
            }

            APPLICATION_SERVICE()->tick( _frameTime );
        }

        APPLICATION_SERVICE()->endUpdate();

        if( updating == false )
        {
            if( m_pauseUpdatingTime < 0.f )
            {
                m_pauseUpdatingTime = _frameTime;
            }

            THREAD_SYSTEM()->sleep( 1 );
        }
        else if( APPLICATION_SERVICE()->getVSync() == false && HAS_OPTION( "maxfps" ) == false )
        {
            THREAD_SYSTEM()->sleep( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::renderPlatform()
    {
        if( this->isNeedWindowRender_() == false || m_window == 0 || m_glContext == nullptr )
        {
            return false;
        }

        if( APPLICATION_SERVICE()->render() == false )
        {
            return false;
        }

        APPLICATION_SERVICE()->flush();

        ::glXSwapBuffers( m_display, m_window );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        if( m_window != 0 )
        {
            ::XUnmapWindow( m_display, m_window );
            ::XFlush( m_display );
        }

        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp UnixPlatformService::getPlatfomTime() const
    {
        return Helper::getSystemTimestamp() - m_beginTime;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp UnixPlatformService::getInputTimestamp() const
    {
        return Helper::getPlatformTimestamp();
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        const Char * projectTitle = _projectTitle != nullptr ? _projectTitle : "";

        if( projectTitle != m_projectTitle.c_str() )
        {
            m_projectTitle.assign( projectTitle );
        }

        if( m_display != nullptr && m_window != 0 )
        {
            const Char * title = m_projectTitle.c_str();
            ::XStoreName( m_display, m_window, title );

            Atom netWmName = ::XInternAtom( m_display, "_NET_WM_NAME", False );
            ::XChangeProperty( m_display, m_window, netWmName, m_utf8StringAtom, 8, PropModeReplace, reinterpret_cast<const unsigned char *>(title), (int)m_projectTitle.size() );
        }

        LOGGER_INFO( "platform", "project title: %s"
            , m_projectTitle.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        return m_projectTitle.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::alreadyRunningMonitor()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & UnixPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        return m_platformTags.hasTag( _tag );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::isDesktop() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::hasTouchpad() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t UnixPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return (int32_t)((float)_dp * m_displayScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t UnixPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        return (int32_t)((float)_dp * m_displayScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t UnixPlatformService::pxToWidthDp( int32_t _px ) const
    {
        return (int32_t)((float)_px / m_displayScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t UnixPlatformService::pxToHeightDp( int32_t _px ) const
    {
        return (int32_t)((float)_px / m_displayScale + 0.5f);
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr UnixPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        UnixDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, "failed to create dynamic library '%s'"
            , _dynamicLibraryName
        );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            return nullptr;
        }

        return dynamicLibrary;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        if( m_display == nullptr )
        {
            return false;
        }

        *_resolution = Resolution( (uint32_t)DisplayWidth( m_display, m_screen ), (uint32_t)DisplayHeight( m_display, m_screen ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        if( m_display == nullptr )
        {
            return false;
        }

        Atom actualType = 0;
        int actualFormat = 0;
        unsigned long itemCount = 0;
        unsigned long bytesAfter = 0;
        unsigned char * data = nullptr;

        int status = ::XGetWindowProperty( m_display, RootWindow( m_display, m_screen ), m_netWorkAreaAtom, 0, 4, False, XA_CARDINAL, &actualType, &actualFormat, &itemCount, &bytesAfter, &data );

        if( status == Success && actualFormat == 32 && itemCount >= 4 && data != nullptr )
        {
            const unsigned long * workArea = reinterpret_cast<const unsigned long *>(data);
            *_resolution = Resolution( (uint32_t)workArea[2], (uint32_t)workArea[3] );
            ::XFree( data );

            return true;
        }

        if( data != nullptr )
        {
            ::XFree( data );
        }

        return this->getDesktopResolution( _resolution );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::isDebuggerPresent() const
    {
        FILE * statusFile = ::fopen( "/proc/self/status", "r" );

        if( statusFile == nullptr )
        {
            return false;
        }

        Char line[256];
        bool attached = false;

        while( ::fgets( line, sizeof( line ), statusFile ) != nullptr )
        {
            int tracerPid = 0;
            if( ::sscanf( line, "TracerPid: %d", &tracerPid ) == 1 )
            {
                attached = tracerPid != 0;
                break;
            }
        }

        ::fclose( statusFile );

        return attached;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::debugBreak()
    {
        ::raise( SIGTRAP );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        ssize_t length = ::readlink( "/proc/self/exe", _currentPath, MENGINE_MAX_PATH - 2 );

        if( length > 0 )
        {
            _currentPath[length] = '\0';

            Char * slash = StdString::strrchr( _currentPath, '/' );
            if( slash != nullptr )
            {
                slash[1] = '\0';

                return StdString::strlen( _currentPath );
            }
        }

        if( ::getcwd( _currentPath, MENGINE_MAX_PATH - 2 ) == nullptr )
        {
            _currentPath[0] = '\0';

            return 0;
        }

        Helper::pathCorrectFolderPathA( _currentPath, MENGINE_PATH_FORWARDSLASH );

        return StdString::strlen( _currentPath );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        PathString folderName = CONFIG_VALUE_PATHSTRING( "Project", "ExtraPreferencesFolderName", "" );

        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( folderName.c_str() ) == true, "invalid extra preferences folder name '%s'"
            , folderName.c_str()
        );

        StdString::strcpy_safe( _folderName, folderName.c_str(), MENGINE_MAX_PATH );

        return folderName.size();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getUserPath( Char * const _userPath ) const
    {
        const Char * dataHome = ::getenv( "XDG_DATA_HOME" );
        const Char * home = ::getenv( "HOME" );

        Path base = {'\0'};

        if( dataHome != nullptr && dataHome[0] == '/' )
        {
            StdString::strcpy_safe( base, dataHome, MENGINE_MAX_PATH );
        }
        else if( home != nullptr && home[0] != '\0' )
        {
            MENGINE_SNPRINTF( base, MENGINE_MAX_PATH, "%s/.local/share", home );
        }
        else
        {
            return 0;
        }

        PathString company = CONFIG_VALUE_PATHSTRING( "Project", "Company", "UNKNOWN" );
        PathString project = CONFIG_VALUE_PATHSTRING( "Project", "Name", "UNKNOWN" );

        MENGINE_SNPRINTF( _userPath, MENGINE_MAX_PATH, "%s/%s/%s/", base, company.c_str(), project.c_str() );

        Path extraFolder = {'\0'};
        if( this->getExtraPreferencesFolderName( extraFolder ) != 0 )
        {
            Helper::pathAppendA( _userPath, extraFolder, MENGINE_PATH_FORWARDSLASH );
        }

        if( Detail::createDirectories( _userPath ) == false )
        {
            LOGGER_ERROR( "failed to create user path '%s'"
                , _userPath
            );

            _userPath[0] = '\0';

            return 0;
        }

        Helper::pathCorrectFolderPathA( _userPath, MENGINE_PATH_FORWARDSLASH );

        return StdString::strlen( _userPath );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getUserLocaleLanguage( Char * const _userLocaleLanguage ) const
    {
        const Char * locale = ::getenv( "LANGUAGE" );

        if( locale == nullptr || locale[0] == '\0' )
        {
            locale = ::getenv( "LC_ALL" );
        }

        if( locale == nullptr || locale[0] == '\0' )
        {
            locale = ::getenv( "LC_MESSAGES" );
        }

        if( locale == nullptr || locale[0] == '\0' )
        {
            locale = ::getenv( "LANG" );
        }

        if( locale == nullptr || locale[0] == '\0' || (locale[0] == 'C' && (locale[1] == '\0' || locale[1] == '.' || locale[1] == '_')) || StdString::strcmp( locale, "POSIX" ) == 0 )
        {
            return false;
        }

        _userLocaleLanguage[0] = locale[0] >= 'A' && locale[0] <= 'Z' ? (Char)(locale[0] - 'A' + 'a') : locale[0];
        _userLocaleLanguage[1] = locale[1] >= 'A' && locale[1] <= 'Z' ? (Char)(locale[1] - 'A' + 'a') : locale[1];
        _userLocaleLanguage[2] = '\0';

        if( locale[1] != '\0' && locale[1] != '_' && locale[1] != '-' )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_INFO( "platform", "create X11 window size %u:%u fullscreen %s"
            , _resolution.getWidth()
            , _resolution.getHeight()
            , _fullscreen == true ? "true" : "false"
        );

        return this->createWindow_( _resolution, _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::createWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_display == nullptr || m_window != 0 )
        {
            return false;
        }

        const int framebufferAttributes[] = {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            None
        };

        int configCount = 0;
        GLXFBConfig * configs = ::glXChooseFBConfig( m_display, m_screen, framebufferAttributes, &configCount );

        if( configs == nullptr || configCount == 0 )
        {
            LOGGER_ERROR( "glXChooseFBConfig failed" );

            if( configs != nullptr )
            {
                ::XFree( configs );
            }

            return false;
        }

        GLXFBConfig config = configs[0];
        XVisualInfo * visualInfo = ::glXGetVisualFromFBConfig( m_display, config );
        ::XFree( configs );

        if( visualInfo == nullptr )
        {
            LOGGER_ERROR( "glXGetVisualFromFBConfig failed" );

            return false;
        }

        Window root = RootWindow( m_display, m_screen );
        m_colormap = ::XCreateColormap( m_display, root, visualInfo->visual, AllocNone );

        XSetWindowAttributes attributes;
        StdString::memset( &attributes, 0, sizeof( attributes ) );
        attributes.colormap = m_colormap;
        attributes.border_pixel = 0;
        attributes.background_pixel = 0;
        attributes.event_mask = KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
            FocusChangeMask | EnterWindowMask | LeaveWindowMask |
            ExposureMask | PropertyChangeMask | StructureNotifyMask;

        Resolution desktopResolution;
        this->getDesktopResolution( &desktopResolution );

        uint32_t width = _fullscreen == true ? desktopResolution.getWidth() : _resolution.getWidth();
        uint32_t height = _fullscreen == true ? desktopResolution.getHeight() : _resolution.getHeight();

        const int x = _fullscreen == true ? 0 : ((int)desktopResolution.getWidth() - (int)width) / 2;
        const int y = _fullscreen == true ? 0 : ((int)desktopResolution.getHeight() - (int)height) / 2;

        m_window = ::XCreateWindow( m_display, root, x, y, width, height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWBorderPixel | CWBackPixel | CWColormap | CWEventMask, &attributes );
        ::XFree( visualInfo );

        if( m_window == 0 )
        {
            LOGGER_ERROR( "XCreateWindow failed" );

            ::XFreeColormap( m_display, m_colormap );
            m_colormap = 0;

            return false;
        }

        ::XSetWMProtocols( m_display, m_window, &m_wmDeleteWindowAtom, 1 );

        XClassHint classHint;
        classHint.res_name = const_cast<Char *>("mengine");
        classHint.res_class = const_cast<Char *>("Mengine");
        ::XSetClassHint( m_display, m_window, &classHint );

        const long pid = (long)::getpid();
        Atom netWmPid = ::XInternAtom( m_display, "_NET_WM_PID", False );
        ::XChangeProperty( m_display, m_window, netWmPid, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char *>(&pid), 1 );

        this->setProjectTitle( m_projectTitle.c_str() );

        if( this->createOpenGLContext_( config ) == false )
        {
            this->destroyWindow_();

            return false;
        }

        if( m_inputMethod != nullptr )
        {
            m_inputContext = ::XCreateIC( m_inputMethod,
                XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                XNClientWindow, m_window,
                XNFocusWindow, m_window,
                nullptr );
        }

        Bool detectableRepeat = False;
        ::XkbSetDetectableAutoRepeat( m_display, True, &detectableRepeat );

        UnixInputPtr input = Helper::makeFactorableUnique<UnixInput>( MENGINE_DOCUMENT_FACTORABLE );
        if( input->initialize( m_display, m_window ) == false )
        {
            this->destroyWindow_();

            return false;
        }

        m_input = input;
        m_windowResolution = Resolution( width, height );
        m_fullscreen = _fullscreen;
        m_windowExposed = true;

        ::XDefineCursor( m_display, m_window, m_cursorVisible == true ? m_defaultCursor : m_hiddenCursor );
        ::XMapRaised( m_display, m_window );

        if( _fullscreen == true )
        {
            this->setFullscreen_( true );
        }

        ::XFlush( m_display );

        this->notifyVsyncChanged( APPLICATION_SERVICE()->getVSync() );

        m_windowAttached = true;
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::createOpenGLContext_( GLXFBConfig _config )
    {
        const Char * extensions = ::glXQueryExtensionsString( m_display, m_screen );
        UnixOpenGLProc createContextAddress = Detail::hasExtension( extensions, "GLX_ARB_create_context" ) == true
            ? this->getOpenGLProcAddress( "glXCreateContextAttribsARB" )
            : nullptr;
        Detail::GLXCreateContextAttribsProc createContext = reinterpret_cast<Detail::GLXCreateContextAttribsProc>(createContextAddress);

        XErrorHandler previousErrorHandler = ::XSetErrorHandler( &Detail::contextErrorHandler );
        Detail::s_contextError = false;

        if( createContext != nullptr )
        {
            const int contextAttributes[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 3,
                GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                None
            };

            m_glContext = createContext( m_display, _config, nullptr, True, contextAttributes );
        }

        ::XSync( m_display, False );
        ::XSetErrorHandler( previousErrorHandler );

        if( Detail::s_contextError == true )
        {
            m_glContext = nullptr;
        }

        if( m_glContext == nullptr )
        {
            LOGGER_WARNING( "OpenGL 3.3 core context creation failed; trying a compatible GLX context" );

            m_glContext = ::glXCreateNewContext( m_display, _config, GLX_RGBA_TYPE, nullptr, True );
        }

        if( m_glContext == nullptr )
        {
            LOGGER_ERROR( "failed to create GLX context" );

            return false;
        }

        if( ::glXMakeCurrent( m_display, m_window, m_glContext ) == False )
        {
            LOGGER_ERROR( "glXMakeCurrent failed" );

            ::glXDestroyContext( m_display, m_glContext );
            m_glContext = nullptr;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::destroyWindow_()
    {
        if( m_windowAttached == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );
            m_windowAttached = false;
        }

        if( m_input != nullptr )
        {
            m_input->finalize();
            m_input = nullptr;
        }

        if( m_inputContext != nullptr )
        {
            ::XDestroyIC( m_inputContext );
            m_inputContext = nullptr;
        }

        if( m_glContext != nullptr )
        {
            ::glXMakeCurrent( m_display, None, nullptr );
            ::glXDestroyContext( m_display, m_glContext );
            m_glContext = nullptr;
        }

        if( m_window != 0 )
        {
            ::XDestroyWindow( m_display, m_window );
            m_window = 0;
        }

        if( m_colormap != 0 )
        {
            ::XFreeColormap( m_display, m_colormap );
            m_colormap = 0;
        }

        m_windowExposed = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::setFullscreen_( bool _fullscreen )
    {
        if( m_window == 0 )
        {
            return;
        }

        XEvent event;
        StdString::memset( &event, 0, sizeof( event ) );
        event.xclient.type = ClientMessage;
        event.xclient.window = m_window;
        event.xclient.message_type = m_netWmStateAtom;
        event.xclient.format = 32;
        event.xclient.data.l[0] = _fullscreen == true ? 1L : 0L;
        event.xclient.data.l[1] = (long)m_netWmStateFullscreenAtom;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 1;

        ::XSendEvent( m_display, RootWindow( m_display, m_screen ), False, SubstructureRedirectMask | SubstructureNotifyMask, &event );
        ::XFlush( m_display );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_window == 0 )
        {
            return true;
        }

        if( SERVICE_IS_INITIALIZE( RenderSystemInterface ) == true )
        {
            RENDER_SYSTEM()->onWindowChangeFullscreenPrepare( _fullscreen );
        }

        this->setFullscreen_( _fullscreen );

        if( _fullscreen == false )
        {
            ::XResizeWindow( m_display, m_window, _resolution.getWidth(), _resolution.getHeight() );
        }

        m_fullscreen = _fullscreen;
        m_windowResolution = _resolution;

        ::XRaiseWindow( m_display, m_window );
        ::XFlush( m_display );

        if( SERVICE_IS_INITIALIZE( RenderSystemInterface ) == true )
        {
            if( RENDER_SYSTEM()->onWindowChangeFullscreen( _fullscreen ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_window == 0 )
        {
            return true;
        }

        if( m_fullscreen == _fullscreen && (_fullscreen == true || m_windowResolution == _resolution) )
        {
            return true;
        }

        return this->changeWindow_( _resolution, _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::notifyVsyncChanged( bool _vsync )
    {
        if( m_display == nullptr || m_window == 0 || m_glContext == nullptr )
        {
            return;
        }

        const Char * extensions = ::glXQueryExtensionsString( m_display, m_screen );

        Detail::GLXSwapIntervalEXTProc swapIntervalEXT = Detail::hasExtension( extensions, "GLX_EXT_swap_control" ) == true
            ? reinterpret_cast<Detail::GLXSwapIntervalEXTProc>(this->getOpenGLProcAddress( "glXSwapIntervalEXT" ))
            : nullptr;

        if( swapIntervalEXT != nullptr )
        {
            swapIntervalEXT( m_display, m_window, _vsync == true ? 1 : 0 );

            return;
        }

        Detail::GLXSwapIntervalMESAProc swapIntervalMESA = Detail::hasExtension( extensions, "GLX_MESA_swap_control" ) == true
            ? reinterpret_cast<Detail::GLXSwapIntervalMESAProc>(this->getOpenGLProcAddress( "glXSwapIntervalMESA" ))
            : nullptr;

        if( swapIntervalMESA != nullptr )
        {
            swapIntervalMESA( _vsync == true ? 1U : 0U );

            return;
        }

        Detail::GLXSwapIntervalSGIProc swapIntervalSGI = Detail::hasExtension( extensions, "GLX_SGI_swap_control" ) == true
            ? reinterpret_cast<Detail::GLXSwapIntervalSGIProc>(this->getOpenGLProcAddress( "glXSwapIntervalSGI" ))
            : nullptr;

        if( swapIntervalSGI != nullptr && _vsync == true )
        {
            swapIntervalSGI( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::closeWindow()
    {
        if( m_window != 0 )
        {
            ::XUnmapWindow( m_display, m_window );
            ::XFlush( m_display );
        }

        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::minimizeWindow()
    {
        if( m_window != 0 )
        {
            ::XIconifyWindow( m_display, m_window, m_screen );
            ::XFlush( m_display );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::setCursorPosition( const mt::vec2f & _cursorPosition )
    {
        if( m_window == 0 )
        {
            return;
        }

        const int x = (int)(_cursorPosition.x * (float)m_windowResolution.getWidth());
        const int y = (int)(_cursorPosition.y * (float)m_windowResolution.getHeight());

        ::XWarpPointer( m_display, None, m_window, 0, 0, 0, 0, x, y );
        ::XFlush( m_display );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::setCursorCapture( bool _capture )
    {
        MENGINE_UNUSED( _capture );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        if( m_window != 0 && m_cursorVisible == true )
        {
            ::XDefineCursor( m_display, m_window, m_defaultCursor );
            ::XFlush( m_display );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::notifyCursorModeChanged( bool _mode )
    {
        m_cursorVisible = _mode;

        if( m_window != 0 )
        {
            ::XDefineCursor( m_display, m_window, _mode == true ? m_defaultCursor : m_hiddenCursor );
            ::XFlush( m_display );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _memory )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _memory );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::showKeyboard()
    {
        m_keyboardVisible = true;

        if( m_inputContext != nullptr )
        {
            ::XSetICFocus( m_inputContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::hideKeyboard()
    {
        m_keyboardVisible = false;

        if( m_inputContext != nullptr )
        {
            ::XUnsetICFocus( m_inputContext );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::isShowKeyboard() const
    {
        return m_keyboardVisible;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );

        *_groupName = ConstString::none();
        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        const Char * arguments[] = {"xdg-open", _url, nullptr};

        if( Detail::launchDetached( "xdg-open", arguments ) == false )
        {
            LOGGER_ERROR( "failed to open URL '%s'"
                , _url
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically )
    {
        MENGINE_UNUSED( _technically );

        String uri = "mailto:";
        Detail::appendUrlEncoded( &uri, _email );
        uri.append( "?subject=" );
        Detail::appendUrlEncoded( &uri, _subject );
        uri.append( "&body=" );
        Detail::appendUrlEncoded( &uri, _body );

        return this->openUrlInDefaultBrowser( uri.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::openDeleteAccount()
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::writeUserFile_( const Char * _baseFolder, const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) const
    {
        const Char * home = ::getenv( "HOME" );

        if( home == nullptr || home[0] == '\0' )
        {
            return false;
        }

        Path directory = {'\0'};
        MENGINE_SNPRINTF( directory, MENGINE_MAX_PATH, "%s/%s/%s", home, _baseFolder, _directoryPath );
        Helper::pathCorrectBackslashA( directory );
        Helper::pathCorrectFolderPathA( directory, MENGINE_PATH_FORWARDSLASH );

        if( Detail::createDirectories( directory ) == false )
        {
            LOGGER_ERROR( "failed to create directory '%s'"
                , directory
            );

            return false;
        }

        Path filePath = {'\0'};
        MENGINE_SNPRINTF( filePath, MENGINE_MAX_PATH, "%s%s", directory, _filePath );

        FILE * file = ::fopen( filePath, "wb" );

        if( file == nullptr )
        {
            LOGGER_ERROR( "failed to open file '%s': %s"
                , filePath
                , ::strerror( errno )
            );

            return false;
        }

        const size_t written = ::fwrite( _data, 1, _size, file );
        const int closeResult = ::fclose( file );

        if( written != _size || closeResult != 0 )
        {
            LOGGER_ERROR( "failed to write file '%s'"
                , filePath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        return this->writeUserFile_( "Pictures", _directoryPath, _filePath, _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        return this->writeUserFile_( "Music", _directoryPath, _filePath, _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );
        int32_t result = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );
        MENGINE_VA_LIST_END( args );

        if( result < 0 )
        {
            StdString::strcpy_safe( message, "invalid message box format", MENGINE_LOGGER_MAX_MESSAGE );
        }

        if( Helper::isSilentDialog() == true )
        {
            LOGGER_MESSAGE( "[messageBox] %s: %s"
                , _caption
                , message
            );

            return;
        }

        const Char * arguments[] = {"zenity", "--info", "--title", _caption, "--text", message, nullptr};

        if( Detail::launchDetached( "zenity", arguments ) == false )
        {
            LOGGER_MESSAGE( "[messageBox] %s: %s"
                , _caption
                , message
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::setClipboardText( const Char * _value ) const
    {
        if( m_display == nullptr || m_clipboardWindow == 0 )
        {
            return false;
        }

        m_clipboardText.assign( _value != nullptr ? _value : "" );

        ::XSetSelectionOwner( m_display, m_clipboardAtom, m_clipboardWindow, CurrentTime );
        ::XFlush( m_display );

        if( ::XGetSelectionOwner( m_display, m_clipboardAtom ) == m_clipboardWindow )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        if( _capacity == 0 || m_display == nullptr || m_clipboardWindow == 0 )
        {
            return false;
        }

        _value[0] = '\0';

        Window owner = ::XGetSelectionOwner( m_display, m_clipboardAtom );

        if( owner == m_clipboardWindow )
        {
            StdString::strcpy_safe( _value, m_clipboardText.c_str(), _capacity );

            return true;
        }

        if( owner == 0 )
        {
            return false;
        }

        ::XDeleteProperty( m_display, m_clipboardWindow, m_clipboardPropertyAtom );
        ::XConvertSelection( m_display, m_clipboardAtom, m_utf8StringAtom, m_clipboardPropertyAtom, m_clipboardWindow, CurrentTime );
        ::XFlush( m_display );

        const Timestamp begin = Helper::getPlatformTimestamp();
        XEvent event;

        for( ;; )
        {
            if( ::XCheckTypedWindowEvent( m_display, m_clipboardWindow, SelectionNotify, &event ) != 0 )
            {
                break;
            }

            if( Helper::getPlatformTimestamp() - begin >= 1000 )
            {
                return false;
            }

            THREAD_SYSTEM()->sleep( 1 );
        }

        if( event.xselection.property == 0 )
        {
            return false;
        }

        Atom actualType = 0;
        int actualFormat = 0;
        unsigned long itemCount = 0;
        unsigned long bytesAfter = 0;
        unsigned char * data = nullptr;

        const long requestedLongs = (long)((_capacity + 3) / 4);
        int status = ::XGetWindowProperty( m_display, m_clipboardWindow, m_clipboardPropertyAtom, 0, requestedLongs, True, AnyPropertyType, &actualType, &actualFormat, &itemCount, &bytesAfter, &data );

        if( status != Success || actualFormat != 8 || data == nullptr )
        {
            if( data != nullptr )
            {
                ::XFree( data );
            }

            return false;
        }

        const size_t copySize = MENGINE_MIN( (size_t)itemCount, _capacity - 1 );
        StdString::strzcpy_safe( _value, reinterpret_cast<const Char *>(data), copySize, _capacity );
        ::XFree( data );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::handleClipboardRequest_( const XSelectionRequestEvent & _request )
    {
        XSelectionEvent response;
        StdString::memset( &response, 0, sizeof( response ) );
        response.type = SelectionNotify;
        response.display = _request.display;
        response.requestor = _request.requestor;
        response.selection = _request.selection;
        response.target = _request.target;
        response.time = _request.time;
        response.property = None;

        Atom property = _request.property != None ? _request.property : _request.target;

        if( _request.target == m_targetsAtom )
        {
            Atom targets[] = {m_targetsAtom, m_utf8StringAtom, m_textPlainAtom, XA_STRING};
            ::XChangeProperty( m_display, _request.requestor, property, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char *>(targets), 4 );
            response.property = property;
        }
        else if( _request.target == m_utf8StringAtom || _request.target == m_textPlainAtom || _request.target == XA_STRING )
        {
            const Atom type = _request.target == XA_STRING ? XA_STRING : m_utf8StringAtom;
            ::XChangeProperty( m_display, _request.requestor, property, type, 8, PropModeReplace, reinterpret_cast<const unsigned char *>(m_clipboardText.c_str()), (int)m_clipboardText.size() );
            response.property = property;
        }

        XEvent event;
        StdString::memset( &event, 0, sizeof( event ) );
        event.xselection = response;

        ::XSendEvent( m_display, _request.requestor, False, 0, &event );
        ::XFlush( m_display );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::processEvents_()
    {
        if( m_display == nullptr )
        {
            return m_quitRequested;
        }

        while( ::XPending( m_display ) > 0 )
        {
            XEvent event;
            ::XNextEvent( m_display, &event );

            if( event.type == SelectionRequest )
            {
                this->handleClipboardRequest_( event.xselectionrequest );

                continue;
            }

            if( event.type == SelectionClear && event.xselectionclear.selection == m_clipboardAtom )
            {
                m_clipboardText.clear();

                continue;
            }

            if( event.type == ClientMessage && (Atom)event.xclient.data.l[0] == m_wmDeleteWindowAtom )
            {
                this->pushQuitEvent_();

                continue;
            }

            switch( event.type )
            {
            case ConfigureNotify:
                {
                    if( event.xconfigure.width > 0 && event.xconfigure.height > 0 )
                    {
                        Resolution resolution( (uint32_t)event.xconfigure.width, (uint32_t)event.xconfigure.height );

                        if( m_windowResolution != resolution )
                        {
                            m_windowResolution = resolution;

                            if( SERVICE_IS_INITIALIZE( ApplicationInterface ) == true )
                            {
                                APPLICATION_SERVICE()->setWindowResolution( resolution );
                            }

                            if( SERVICE_IS_INITIALIZE( RenderSystemInterface ) == true )
                            {
                                RENDER_SYSTEM()->onWindowMovedOrResized();
                            }
                        }
                    }
                }break;
            case Expose:
                m_windowExposed = true;
                break;
            case FocusIn:
                if( m_inputContext != nullptr )
                {
                    ::XSetICFocus( m_inputContext );
                }

                this->setActive_( true, this->getInputTimestamp() );
                break;
            case FocusOut:
                if( m_inputContext != nullptr )
                {
                    ::XUnsetICFocus( m_inputContext );
                }

                if( m_input != nullptr )
                {
                    m_input->releaseAll();
                }

                this->setActive_( false, this->getInputTimestamp() );
                break;
            case MapNotify:
                m_windowExposed = true;
                break;
            case UnmapNotify:
                m_windowExposed = false;

                if( m_input != nullptr )
                {
                    m_input->releaseAll();
                }

                this->setActive_( false, this->getInputTimestamp() );
                break;
            case DestroyNotify:
                this->pushQuitEvent_();
                break;
            default:
                break;
            }

            if( ::XFilterEvent( &event, m_window ) != 0 )
            {
                continue;
            }

            if( event.type == KeyRelease && Detail::isKeyRepeat( m_display, event ) == true )
            {
                continue;
            }

            Char eventTextBuffer[256 + 1] = {'\0'};
            const Char * eventText = this->getEventText_( event, eventTextBuffer, sizeof( eventTextBuffer ) ) != 0
                ? eventTextBuffer
                : nullptr;

            bool handled = false;

            for( const UnixEventHandlerDesc & desc : m_eventHandlers )
            {
                if( desc.lambda( event, eventText ) == true )
                {
                    handled = true;
                    break;
                }
            }

            const bool releaseEvent = event.type == KeyRelease || event.type == ButtonRelease;

            if( (handled == false || releaseEvent == true) && m_input != nullptr )
            {
                m_input->handleEvent( event, eventText );
            }

            if( handled == false && event.type == KeyPress && (event.xkey.state & Mod1Mask) != 0 )
            {
                KeySym keySym = ::XkbKeycodeToKeysym( m_display, (KeyCode)event.xkey.keycode, 0, 0 );

                if( (keySym == XK_Return || keySym == XK_KP_Enter) && APPLICATION_SERVICE()->getAllowFullscreenSwitchShortcut() == true )
                {
                    APPLICATION_SERVICE()->setFullscreenMode( APPLICATION_SERVICE()->getFullscreenMode() == false );
                }
            }
        }

        return m_quitRequested;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t UnixPlatformService::getEventText_( const XEvent & _event, Char * const _text, size_t _capacity ) const
    {
        if( _event.type != KeyPress || _capacity <= 1 )
        {
            return 0;
        }

        XKeyEvent keyEvent = _event.xkey;
        const int textCapacity = (int)MENGINE_MIN( _capacity - 1, (size_t)INT_MAX );
        int textSize = 0;

        if( m_inputContext != nullptr )
        {
            Status status = 0;
            KeySym keySym = 0;
            textSize = ::Xutf8LookupString( m_inputContext, &keyEvent, _text, textCapacity, &keySym, &status );

            if( status == XBufferOverflow || status == XLookupNone || status == XLookupKeySym )
            {
                textSize = 0;
            }
        }
        else
        {
            textSize = ::XLookupString( &keyEvent, _text, textCapacity, nullptr, nullptr );
        }

        if( textSize <= 0 )
        {
            _text[0] = '\0';

            return 0;
        }

        _text[textSize] = '\0';

        return (size_t)textSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::pushQuitEvent_()
    {
        if( m_quitRequested == true )
        {
            return;
        }

        m_quitRequested = true;

        if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( ApplicationInterface ) == true )
        {
            APPLICATION_SERVICE()->quit();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::setActive_( bool _active, Timestamp _timestamp )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        if( SERVICE_IS_INITIALIZE( ApplicationInterface ) == false || SERVICE_IS_INITIALIZE( InputServiceInterface ) == false )
        {
            return;
        }

        const bool nopause = APPLICATION_SERVICE()->getNopause();

        mt::vec2f point( 0.f, 0.f );
        if( m_input != nullptr )
        {
            m_input->getCursorPosition( &point );
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
            APPLICATION_SERVICE()->setFocus( m_active );
            INPUT_SERVICE()->onFocus( m_active );
            APPLICATION_SERVICE()->turnSound( m_active );
        }
        else
        {
            APPLICATION_SERVICE()->setFocus( true );
            INPUT_SERVICE()->onFocus( true );
            APPLICATION_SERVICE()->turnSound( true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::isNeedWindowRender_() const
    {
        if( m_windowExposed == false )
        {
            return false;
        }

        if( APPLICATION_SERVICE()->getNopause() == true )
        {
            return true;
        }

        if( m_active == true && APPLICATION_SERVICE()->isFocus() == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Display * UnixPlatformService::getX11Display() const
    {
        return m_display;
    }
    //////////////////////////////////////////////////////////////////////////
    Window UnixPlatformService::getX11Window() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    UnixOpenGLProc UnixPlatformService::getOpenGLProcAddress( const Char * _name ) const
    {
        __GLXextFuncPtr address = ::glXGetProcAddressARB( reinterpret_cast<const GLubyte *>(_name) );

        if( address != nullptr )
        {
            return reinterpret_cast<UnixOpenGLProc>(address);
        }

        void * symbol = ::dlsym( RTLD_DEFAULT, _name );
        UnixOpenGLProc proc = nullptr;

        static_assert( sizeof( proc ) == sizeof( symbol ), "function and data pointers must have equal size" );
        StdString::memcpy( &proc, &symbol, sizeof( proc ) );

        return proc;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId UnixPlatformService::addUnixEventHandler( const LambdaUnixEventHandler & _lambda, const DocumentInterfacePtr & _doc )
    {
        UniqueId id = ENUMERATOR_SERVICE()->generateUniqueIdentity();

        UnixEventHandlerDesc desc;
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
    void UnixPlatformService::removeUnixEventHandler( UniqueId _id )
    {
        VectorUnixEventHandlers::iterator it = StdAlgorithm::find_if( m_eventHandlers.begin(), m_eventHandlers.end(), [_id]( const UnixEventHandlerDesc & _desc )
        {
            if( _desc.id == _id )
            {
                return true;
            }

            return false;
        } );

        MENGINE_ASSERTION_FATAL( it != m_eventHandlers.end(), "Unix event handler '%u' was not found"
            , _id
        );

        m_eventHandlers.erase( it );
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixPlatformService::initializeFileService()
    {
        FileGroupInterfacePtr defaultFileGroup = nullptr;

        if( FILE_SERVICE()->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &defaultFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none(), defaultFileGroup, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FileGroupInterfacePtr developmentFileGroup = nullptr;

        if( FILE_SERVICE()->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &developmentFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount development directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), developmentFileGroup, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixPlatformService::finalizeFileService()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FILE_SERVICE()->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()->unmountFileGroup( ConstString::none() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
}
