#   include "Config/Config.h"

#   include "SDLPlatform.h"

#   include "Interface/LoggerInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/OptionsInterface.h"
#   include "Interface/TimerInterface.h"

#	ifdef WIN32
#	include "WIN32/WindowsIncluder.h"
#	endif

#	ifdef __APPLE__
#	include "TargetConditionals.h"
#	endif

#   include <cstdio>
#   include <clocale>

#   include "Core/FileLogger.h"
#   include "Core/IniUtil.h"
#   include "Core/FilePath.h"

#   include "Factory/FactorableUnique.h"
#   include "Factory/FactoryDefault.h"

#   include "Logger/Logger.h"

#   include <ctime>
#   include <algorithm>

#   include <sstream>
#   include <iomanip>

#   include <sys/stat.h>

#   define PARAM_UNUSED(x) ((void)x)

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Mengine::SDLPlatform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::SDLPlatform()
        : m_window( nullptr )
        , m_glContext( nullptr )
        , m_sdlInput( nullptr )
        , m_icon( 0 )
        , m_shouldQuit( false )
        , m_running( false )
        , m_pause( false )
        , m_touchpad( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::~SDLPlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t SDLPlatform::getCurrentPath( WChar * _path, size_t _len ) const
    {
#ifdef WIN32
        DWORD len = (DWORD)::GetCurrentDirectory( (DWORD)_len, _path );

        if( len == 0 )
        {
            return 0;
        }

        _path[len] = L'/';
        _path[len + 1] = L'\0';

        Helper::pathCorrectBackslash( _path, _path );

        return (size_t)len + 1;
#elif TARGET_OS_IPHONE
        wcscpy( _path, L"deploy-ios-data/" );

        return 16;
#else  
        _path[0] = L'\0';

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getUserPath( WChar * _path, size_t _len ) const
    {
        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        FilePath userPath;
        if( (developmentMode == true && roamingMode == false) || noroamingMode == true )
        {
            WChar currentPathW[MENGINE_MAX_PATH];
            size_t currentPathLen = PLATFORM_SERVICE()
                ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

            if( _len <= currentPathLen + 5 )
            {
                return 0;
            }

            wcscpy( _path, currentPathW );
            wcscat( _path, L"User" );
            wcscat( _path, L"/" );

            return wcslen( _path );
        }
        else
        {
            WString Project_Company = CONFIG_VALUE( "Project", "Company", L"NONAME" );
            WString Project_Name = CONFIG_VALUE( "Project", "Name", L"UNKNOWN" );

            String utf8_Project_Company;
            Helper::unicodeToUtf8( Project_Company, utf8_Project_Company );

            String utf8_Project_Name;
            Helper::unicodeToUtf8( Project_Name, utf8_Project_Name );

            char * sdl_prefPath = SDL_GetPrefPath( utf8_Project_Company.c_str(), utf8_Project_Name.c_str() );

            WString unicode_UserPath;
            Helper::utf8ToUnicode( sdl_prefPath, unicode_UserPath );

            SDL_free( sdl_prefPath );

            if( _len <= unicode_UserPath.size() )
            {
                return 0;
            }

            Helper::pathCorrectBackslash( _path, unicode_UserPath.c_str() );

            return unicode_UserPath.size();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getShortPathName( const WString & _path, WChar * _short, size_t _len ) const
    {
        size_t pathSize = _path.size();

        if( _len == pathSize )
        {
            wcscpy( _short, _path.c_str() );
        }

        return _path.size();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::getMaxClientResolution( Resolution & _resolution ) const
    {
        SDL_Rect rect;
        SDL_GetDisplayBounds( 0, &rect );

        LOGGER_WARNING( "SDLPlatform::getMaxClientResolution w %d h %d"
            , rect.w
            , rect.h
        );

        _resolution = Resolution( (uint32_t)rect.w, (uint32_t)rect.h );
    }
    //////////////////////////////////////////////////////////////////////////
    static void MySDL_LogOutputFunction( void *userdata, int category, SDL_LogPriority priority, const char *message )
    {
        (void)userdata;
        (void)category;

        EMessageLevel level = LM_INFO;

        switch( priority )
        {
        case SDL_LOG_PRIORITY_VERBOSE:
            level = LM_INFO;
            break;
        case SDL_LOG_PRIORITY_DEBUG:
            level = LM_WARNING;
            break;
        case SDL_LOG_PRIORITY_INFO:
            level = LM_INFO;
            break;
        case SDL_LOG_PRIORITY_WARN:
            level = LM_WARNING;
            break;
        case SDL_LOG_PRIORITY_ERROR:
            level = LM_ERROR;
            break;
        case SDL_LOG_PRIORITY_CRITICAL:
            level = LM_CRITICAL;
            break;
        default:
            break;
        }

        size_t messageLen = strlen( message );

        LOGGER_SERVICE()
            ->logMessage( level, 0, message, messageLen );
    }
    //////////////////////////////////////////////////////////////////////////
    static int RemoveMouse_EventFilter( void *userdata, SDL_Event * event )
    {
        (void)userdata;

        switch( event->type )
        {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            return 0;
        default: break;
        };

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::_initializeService()
    {
        if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        {
            LOGGER_CRITICAL( "SDL initialization failed" );
            return false;
        }

        const Char* sdlPlatform = SDL_GetPlatform();
        const int sdlRam = SDL_GetSystemRAM();

        if( strcmp( sdlPlatform, "Windows" ) == 0 )
        {
            m_touchpad = false;
            m_platformName.addTag( Helper::stringizeString( "WIN32" ) );
        }
        else if( strcmp( sdlPlatform, "Mac OS X" ) == 0 )
        {
            m_touchpad = false;
            m_platformName.addTag( Helper::stringizeString( "MAC" ) );
        }
        else if( strcmp( sdlPlatform, "Android" ) == 0 )
        {
            m_touchpad = true;
            m_platformName.addTag( Helper::stringizeString( "ANDROID" ) );
            SDL_SetEventFilter( RemoveMouse_EventFilter, nullptr );
        }
        else if( strcmp( sdlPlatform, "iOS" ) == 0 )
        {
            m_touchpad = true;
            m_platformName.addTag( Helper::stringizeString( "IOS" ) );
            SDL_SetEventFilter( RemoveMouse_EventFilter, nullptr );
        }

        LOGGER_WARNING( "Device info:"
        );

        LOGGER_WARNING( "Platform: %s"
            , sdlPlatform
        );

        LOGGER_WARNING( "RAM: %d MB"
            , sdlRam
        );

        SDL_LogSetOutputFunction( &MySDL_LogOutputFunction, nullptr );
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_WARN );


        m_sdlInput = new FactorableUnique<SDLInput>();

        if( m_sdlInput->initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::_finalizeService()
    {
        this->destroyWindow_();

        SDL_Quit();

        if( m_sdlInput != nullptr )
        {
            m_sdlInput->finalize();
            m_sdlInput = nullptr;
        }

        m_platformName.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::update()
    {
        TIMER_SERVICE()
            ->resetDeltaTime();

        while( true )
        {
            float frameTime = TIMER_SERVICE()
                ->getDeltaTime();

            if( m_pause == true )
            {
                SDL_Delay( 100 );

                continue;
            }

            bool quitRequest = this->processEvents();

            if( quitRequest == true )
            {
                break;
            }

            bool updating = APPLICATION_SERVICE()
                ->beginUpdate();

            if( updating == true )
            {
                APPLICATION_SERVICE()
                    ->tick( frameTime );
            }
            else
            {
                SDL_Delay( 20 );
            }

            if( APPLICATION_SERVICE()
                ->isFocus() == true )
            {
                if( APPLICATION_SERVICE()
                    ->render() == true )
                {
                    APPLICATION_SERVICE()
                        ->flush();

                    SDL_GL_SwapWindow( m_window );
                }
            }

            APPLICATION_SERVICE()
                ->endUpdate();

            SDL_Delay( 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::openUrlInDefaultBrowser( const WString & _url )
    {
        PARAM_UNUSED( _url );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::stopPlatform()
    {
        m_running = false;
        m_shouldQuit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setIcon( uint32_t _icon )
    {
        m_icon = _icon;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SDLPlatform::getIcon() const
    {
        return m_icon;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setProjectTitle( const WString & _projectTitle )
    {
        m_projectTitle = _projectTitle;
    }
    //////////////////////////////////////////////////////////////////////////
    const WString & SDLPlatform::getProjectTitle() const
    {
        return m_projectTitle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_WARNING( "SDLPlatform::createWindow w %d h %d fullscreen %d"
            , _resolution.getWidth()
            , _resolution.getHeight()
            , _fullscreen
        );

        if( this->createWindow_( _resolution, _fullscreen ) == false )
        {
            return false;
        }

        m_glContext = SDL_GL_CreateContext( m_window );

        if( m_glContext == nullptr )
        {
            SDL_DestroyWindow( m_window );
            m_window = nullptr;

            return false;
        }

        int dw;
        int dh;
        SDL_GL_GetDrawableSize( m_window, &dw, &dh );

        m_sdlInput->updateSurfaceResolution( static_cast<float>(dw),
            static_cast<float>(dh) );

        ;

#if TARGET_OS_IPHONE
        APPLICATION_SERVICE()
            ->changeWindowResolution( Resolution( dw, dh ) );
#endif

#if defined(__ANDROID__)
        APPLICATION_SERVICE()
            ->changeWindowResolution( Resolution( dw, dh ) );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer SDLPlatform::getWindowHandle() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasPlatformTag( const ConstString & _tag ) const
    {
        return m_platformName.hasTag( _tag );
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & SDLPlatform::getPlatformTags() const
    {
        return m_platformName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::getDesktopResolution( Resolution & _resolution ) const
    {
#if TARGET_OS_IPHONE
        if( m_window == nullptr )
        {
            return false;
        }

        int dw;
        int dh;
        SDL_GL_GetDrawableSize( m_window, &dw, &dh );

        _resolution.setWidth( static_cast<uint32_t>(dw) );
        _resolution.setHeight( static_cast<uint32_t>(dh) );
#elif defined(__ANDROID__)
        if( m_window == nullptr )
        {
            return false;
        }

        int dw;
        int dh;
        SDL_GL_GetDrawableSize( m_window, &dw, &dh );

        _resolution.setWidth( static_cast<uint32_t>(dw) );
        _resolution.setHeight( static_cast<uint32_t>(dh) );
#else
        SDL_DisplayMode dm;
        if( SDL_GetDesktopDisplayMode( 0, &dm ) != 0 )
        {
            LOGGER_ERROR( "SDLPlatform::getDesktopResolution failed %s"
                , SDL_GetError()
            );

            return false;
        }

        _resolution.setWidth( static_cast<uint32_t>(dm.w) );
        _resolution.setHeight( static_cast<uint32_t>(dm.h) );
#endif

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
#if TARGET_OS_IPHONE
        return true;
#elif defined(__ANDROID__)
        return true;
#endif

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::minimizeWindow()
    {
        SDL_MinimizeWindow( m_window );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setCursorPosition( const mt::vec2f & _pos )
    {
        Resolution resolution;
        if( this->getDesktopResolution( resolution ) == false )
        {
            return;
        }

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        const int wndPosX = static_cast<int>(_pos.x * width);
        const int wndPosY = static_cast<int>(_pos.y * height);

        // ! This function generates a mouse motion event !
        SDL_WarpMouseInWindow( m_window, wndPosX, wndPosY );
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
    void SDLPlatform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        if( m_window == nullptr )
        {
            return;
        }

        Uint32 flags = SDL_GetWindowFlags( m_window );

        if( _fullscreen == true && !(flags & SDL_WINDOW_FULLSCREEN) )
        {
            this->changeWindow_( _resolution, _fullscreen );
        }
        else if( _fullscreen == false && (flags & SDL_WINDOW_FULLSCREEN) )
        {
            this->changeWindow_( _resolution, _fullscreen );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyVsyncChanged( bool _vsync )
    {
        PARAM_UNUSED( _vsync );

        if( _vsync == false )
        {
            SDL_GL_SetSwapInterval( 0 );
        }
        else
        {
            SDL_GL_SetSwapInterval( 1 );
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
    bool SDLPlatform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
    {
        PARAM_UNUSED( _name );
        PARAM_UNUSED( _path );
        PARAM_UNUSED( _buffer );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::onEvent( const ConstString & _event, const TMapWParams & _params )
    {
        PARAM_UNUSED( _event );
        PARAM_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_createDurectoryFullpath( const WChar * _fullpath )
    {
#if defined(__APPLE__)
        Char utf8_fullpath[MENGINE_MAX_PATH];
        Helper::unicodeToUtf8( _fullpath, utf8_fullpath, MENGINE_MAX_PATH );

        int status = mkdir( utf8_fullpath, 0700 );

        if( status != 0 )
        {
            LOGGER_WARNING( "SDLPlatform::createDirectory %ls alredy exists"
                , _fullpath
            );

            return false;
        }

#elif defined(__ANDROID__)
        Char utf8_fullpath[MENGINE_MAX_PATH];
        Helper::unicodeToUtf8( _fullpath, utf8_fullpath, MENGINE_MAX_PATH );

        int status = mkdir( utf8_fullpath, 0700 );

        if( status != 0 )
        {
            LOGGER_WARNING( "SDLPlatform::createDirectory %ls alredy exists"
                , _fullpath
            );

            return false;
        }

#elif defined(WIN32)
        BOOL successful = CreateDirectoryW( _fullpath, NULL );

        if( successful == FALSE )
        {
            DWORD err = GetLastError();

            switch( err )
            {
            case ERROR_ALREADY_EXISTS:
                {
                    LOGGER_WARNING( "SDLPlatform::createDirectory %ls alredy exists"
                        , _fullpath
                    );

                    return false;
                }break;
            case ERROR_PATH_NOT_FOUND:
                {
                    LOGGER_WARNING( "SDLPlatform::createDirectory %ls not found"
                        , _fullpath
                    );

                    return false;
                }break;
            default:
                {
                    LOGGER_WARNING( "SDLPlatform::createDirectory %ls unknown error %d"
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
    static bool s_isDirectoryFullpath( const WChar * _fullpath )
    {
        Char utf8_fullpath[MENGINE_MAX_PATH];
        Helper::unicodeToUtf8( _fullpath, utf8_fullpath, MENGINE_MAX_PATH );

        struct stat sb;
        if( stat( utf8_fullpath, &sb ) == 0 && ((sb.st_mode)& S_IFMT) == S_IFDIR )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::existDirectory( const WString & _path ) const
    {
        WChar fullPath[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( fullPath, _path.c_str() );

        Helper::pathRemoveFileSpec( fullPath );

        size_t len = wcslen( fullPath );

        if( len == 0 )	// current dir
        {
            return true;
        }

        if( fullPath[len - 1] == L':' )	// root dir
        {
            return true;	// let it be
        }

        bool exist = s_isDirectoryFullpath( fullPath );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectory( const WString & _path )
    {
        WChar fullPath[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( fullPath, _path.c_str() );

        Helper::pathRemoveFileSpec( fullPath );

        if( s_isDirectoryFullpath( fullPath ) == true )
        {
            return true;
        }

        Helper::pathRemoveBackslash( fullPath );

        TVectorWString paths;

        for( ;; )
        {
            paths.push_back( fullPath );

            if( Helper::pathRemoveFileSpec( fullPath ) == false )
            {
                break;
            }

            Helper::pathRemoveBackslash( fullPath );

            if( s_isDirectoryFullpath( fullPath ) == true )
            {
                break;
            }
        }

        for( TVectorWString::reverse_iterator
            it = paths.rbegin(),
            it_end = paths.rend();
            it != it_end;
            ++it )
        {
            const WString & path = *it;

            if( s_createDurectoryFullpath( path.c_str() ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::removeFile( const WString & _path )
    {
        WChar userPath[MENGINE_MAX_PATH];
        this->getUserPath( userPath, MENGINE_MAX_PATH );

        WChar pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslash( pathCorrect, _path.c_str() );

        WChar fullPath[MENGINE_MAX_PATH];
        wcscpy( fullPath, userPath );
        wcscat( fullPath, pathCorrect );

        Char utf8_fullpath[MENGINE_MAX_PATH];
        Helper::unicodeToUtf8( fullPath, utf8_fullpath, MENGINE_MAX_PATH );

        int result = remove( utf8_fullpath );

        if( result != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#	ifdef WIN32
    //////////////////////////////////////////////////////////////////////////
    static time_t s_FileTimeToUnixTime( const FILETIME * filetime )
    {
        unsigned int a0;			/* 16 bit, low    bits */
        unsigned int a1;			/* 16 bit, medium bits */
        unsigned int a2;			/* 32 bit, high   bits */

        unsigned int carry;		/* carry bit for subtraction */
        int negative;		/* whether a represents a negative value */

                            /* Copy the time values to a2/a1/a0 */
        a2 = filetime->dwHighDateTime;
        a1 = ((unsigned int)filetime->dwLowDateTime) >> 16;
        a0 = ((unsigned int)filetime->dwLowDateTime) & 0xffff;

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
        negative = (a2 >= ((unsigned int)1) << 31);
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
#	endif
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLPlatform::getFileTime( const WString & _filePath ) const
    {
#	ifdef WIN32	
        const WChar * str_filePath = _filePath.c_str();

        HANDLE handle = ::CreateFile( str_filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( handle, &creation, &access, &write ) == FALSE )
        {
            ::CloseHandle( handle );

            return 0U;
        }

        time_t time = s_FileTimeToUnixTime( &write );

        ::CloseHandle( handle );

        return time;
#	else
        return 0U;
#	endif		
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        PARAM_UNUSED( _path );
        PARAM_UNUSED( _file );
        PARAM_UNUSED( _data );
        PARAM_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        PARAM_UNUSED( _path );
        PARAM_UNUSED( _file );
        PARAM_UNUSED( _data );
        PARAM_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
#if defined(TARGET_OS_IPHONE)
        RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen );
#elif defined(__ANDROID__)
        RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen );
#else
        //RENDER_SERVICE()
        //    ->destroyRenderWindow();

        //SDL_Window * old_window = m_window;
        //SDL_HideWindow( old_window );
        //SDL_GL_MakeCurrent( old_window, nullptr );

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        RENDER_SYSTEM()
            ->onWindowChangeFullscreenPrepare( _fullscreen );

        this->destroyWindow_();
        this->createWindow( _resolution, _fullscreen );

        RENDER_SERVICE()
            ->onDeviceLostRestore();

        RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        Mengine::Char utf8Title[1024] = { 0 };
        Helper::unicodeToUtf8( m_projectTitle, utf8Title, 1024 );

        //SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
        SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

        Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

#if defined(TARGET_OS_IPHONE)
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );

        //SDL_SetHint( SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight" );
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );

#elif defined(__ANDROID__)
        //windowFlags |= SDL_WINDOW_FULLSCREEN;

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );

        SDL_SetHint( SDL_HINT_ORIENTATIONS, "Portrait" );
        //SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );

#else
        if( _fullscreen == true )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
#endif

#if defined(TARGET_OS_IPHONE)
        m_window = SDL_CreateWindow( utf8Title
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#elif defined(__ANDROID__)
        m_window = SDL_CreateWindow( utf8Title
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#else

        int width = static_cast<int>(_resolution.getWidth());
        int height = static_cast<int>(_resolution.getHeight());

        m_window = SDL_CreateWindow( utf8Title
            , SDL_WINDOWPOS_CENTERED
            , SDL_WINDOWPOS_CENTERED
            , width
            , height
            , windowFlags
        );
#endif

        if( m_window == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::destroyWindow_()
    {
        if( m_glContext != nullptr )
        {
            SDL_GL_DeleteContext( m_glContext );
            m_glContext = nullptr;
        }

        if( m_window != nullptr )
        {
            SDL_DestroyWindow( m_window );
            m_window = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::processEvents()
    {
        bool quitRequest = m_shouldQuit;

        if( m_shouldQuit == true )
        {
            return true;
        }

        Uint32 windowID = SDL_GetWindowID( m_window );

        SDL_Event sdlEvent;
        while( SDL_PollEvent( &sdlEvent ) != 0 && !quitRequest )
        {
            m_sdlInput->handleEvent( sdlEvent );

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
                    }
                }break;
            case SDL_WINDOWEVENT:
                {
                    if( sdlEvent.window.windowID != windowID )
                    {
                        continue;
                    }

                    SDL_WindowEventID windowEventId = (SDL_WindowEventID)sdlEvent.window.event;

                    switch( windowEventId )
                    {
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                        {
                            this->setActive_( true );
                        }break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        {
                            this->setActive_( false );
                        }break;
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                            SDL_Event newEvent;
                            newEvent = sdlEvent;
                            newEvent.type = SDL_QUIT;

                            SDL_PushEvent( &newEvent );
                        }break;
                    default:
                        break;
                    }
                }break;
            case SDL_QUIT:
                {
                    return true;
                }break;
            }
        }

        return quitRequest;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setActive_( bool _active )
    {
        APPLICATION_SERVICE()
            ->setFocus( _active );

        INPUT_SERVICE()
            ->onFocus( _active );

        APPLICATION_SERVICE()
            ->turnSound( _active );
    }
}
