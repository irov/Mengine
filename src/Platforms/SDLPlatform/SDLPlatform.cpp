#include "Config/Config.h"

#include "SDLPlatform.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "SDLDynamicLibrary.h"
#include "SDLDateTimeProvider.h"

#include "Kernel/FileLogger.h"
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

#include "Config/StdString.h"

#if defined(MENGINE_PLATFORM_OSX) || defined(MENGINE_PLATFORM_IOS)
#   include "TargetConditionals.h"
#endif

#include <cstdio>
#include <clocale>

#include <ctime>
#include <algorithm>

#include <iomanip>

#include <sys/stat.h>

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
SERVICE_FACTORY( Platform, Mengine::SDLPlatform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::SDLPlatform()
        : m_window( nullptr )
        , m_accelerometer( nullptr )
        , m_enumerator( 0 )
        , m_glContext( nullptr )
        , m_sdlInput( nullptr )
        , m_icon( 0 )
        , m_prevTime( 0 )
        , m_shouldQuit( false )
        , m_running( false )
        , m_pause( false )
        , m_desktop( false )
        , m_touchpad( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::~SDLPlatform()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t SDLPlatform::getCurrentPath( Char * _currentPath ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        WChar unicode_path[MENGINE_MAX_PATH];
        DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH, unicode_path );

        if( len == 0 )
        {
            return 0;
        }

        unicode_path[len] = L'/';
        unicode_path[len + 1] = L'\0';

        Helper::pathCorrectBackslashToW( unicode_path, unicode_path );

        size_t pathLen;
        if( Helper::unicodeToUtf8( unicode_path, _currentPath, MENGINE_MAX_PATH, &pathLen ) == false )
        {
            return 0;
        }

        return pathLen;
#elif defined(MENGINE_PLATFORM_IOS)
        const char deploy_ios_data[] = "deploy-ios-data/";
        MENGINE_STRCPY( _currentPath, deploy_ios_data );

        return sizeof( deploy_ios_data ) - 1;
#else  
        _currentPath[0] = L'\0';

        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getUserPath( Char * _userPath ) const
    {
        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        FilePath userPat1h;
        FilePath userPat2h;
        if( (developmentMode == true && roamingMode == false) || noroamingMode == true )
        {
            Char currentPath[MENGINE_MAX_PATH];
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

        const Char * Project_Company = CONFIG_VALUE( "Project", "Company", "NONAME" );
        const Char * Project_Name = CONFIG_VALUE( "Project", "Name", "UNKNOWN" );

        char * sdl_prefPath = SDL_GetPrefPath( Project_Company, Project_Name );

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
    size_t SDLPlatform::getUserName( Char * _userName ) const
    {
        _userName[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    float SDLPlatform::getJoystickAxis( uint32_t _index ) const
    {
        if( m_accelerometer == nullptr )
        {
            return 0.f;
        }

        float axis = SDL_JoystickGetAxis( m_accelerometer, _index );

        const float inv_maxint32f = 1.f / 32767.f;

        axis *= inv_maxint32f;

#if defined(MENGINE_PLATFORM_IOS)
        axis *= SDL_IPHONE_MAX_GFORCE;
#endif
        return axis;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getShortPathName( const Char * _path, Char * _short, size_t _len ) const
    {
        size_t pathSize = MENGINE_STRLEN( _path );

        if( _len == pathSize )
        {
            MENGINE_STRCPY( _short, _path );
        }

        return pathSize;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SDLPlatform::getSystemFontPath( const Char * _fontName, Char * _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::getMaxClientResolution( Resolution & _resolution ) const
    {
        SDL_Rect rect;
        SDL_GetDisplayUsableBounds( 0, &rect );

        LOGGER_MESSAGE( "client resolution width %d height %d"
            , rect.w
            , rect.h
        );

        _resolution = Resolution( (uint32_t)rect.w, (uint32_t)rect.h );
    }
    //////////////////////////////////////////////////////////////////////////
    static void MySDL_LogOutputFunction( void * _userdata, int _category, SDL_LogPriority _priority, const char * _message )
    {
        MENGINE_UNUSED( _userdata );
        MENGINE_UNUSED( _category );

        ELoggerLevel level = LM_INFO;

        switch( _priority )
        {
        case SDL_LOG_PRIORITY_VERBOSE:
            level = LM_INFO;
            break;
        case SDL_LOG_PRIORITY_DEBUG:
            level = LM_MESSAGE;
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

        size_t messageLen = MENGINE_STRLEN( _message );

        LOGGER_SERVICE()
            ->logMessage( level, 0, LCOLOR_NONE, _message, messageLen );
    }
    //////////////////////////////////////////////////////////////////////////
    static int s_RemoveMouse_EventFilter( void * userdata, SDL_Event * event )
    {
        MENGINE_UNUSED( userdata );

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
        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
        {
            return false;
        }

        const Char * sdlPlatform = SDL_GetPlatform();

        if( MENGINE_STRCMP( sdlPlatform, "Windows" ) == 0 )
        {
            m_desktop = true;
            m_touchpad = false;
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "PC" ) );
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "WIN32" ) );
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
            SDL_SetEventFilter( &s_RemoveMouse_EventFilter, nullptr );
        }
        else if( MENGINE_STRCMP( sdlPlatform, "iOS" ) == 0 )
        {
            m_desktop = false;
            m_touchpad = true;
            m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "IOS" ) );
            SDL_SetEventFilter( &s_RemoveMouse_EventFilter, nullptr );
        }
        else
        {
            LOGGER_ERROR( "SDL Platform '%s' unspecified"
                , sdlPlatform
            );
        }

        if( HAS_OPTION( "touchpad" ) == true )
        {
            m_touchpad = true;
        }

        if( HAS_OPTION( "desktop" ) == true )
        {
            m_desktop = true;
        }

        SDL_LogSetOutputFunction( &MySDL_LogOutputFunction, nullptr );

#ifdef MENGINE_DEBUG
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_INFO );
#else
        SDL_LogSetAllPriority( SDL_LOG_PRIORITY_ERROR );
#endif

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

        LOGGER_MESSAGE( "Android SDK version: %d", AndroidSDKVersion );
        LOGGER_MESSAGE( "Android TV: %d", AndroidTV );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::_runService()
    {
        DateTimeProviderInterfacePtr dateTimeProvider =
            this->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        LOGGER_MESSAGE( "Date: %02d.%02d.%d, %02d:%02d:%02d"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        LOGGER_MESSAGE( "Device info:"
        );

        LOGGER_MESSAGE( "Platform: %s"
            , SDL_GetPlatform()
        );

        LOGGER_MESSAGE( "CPU: %d Count %d CacheLineSize"
            , SDL_GetCPUCount()
            , SDL_GetCPUCacheLineSize()
        );

        LOGGER_MESSAGE( "CPU RDTSC: %d"
            , SDL_HasRDTSC()
        );

        LOGGER_MESSAGE( "CPU AltiVec: %d"
            , SDL_HasAltiVec()
        );

        LOGGER_MESSAGE( "CPU MMX: %d"
            , SDL_HasMMX()
        );

        LOGGER_MESSAGE( "CPU 3DNow: %d"
            , SDL_Has3DNow()
        );

        LOGGER_MESSAGE( "CPU SSE: %d"
            , SDL_HasSSE()
        );

        LOGGER_MESSAGE( "CPU SSE2: %d"
            , SDL_HasSSE2()
        );

        LOGGER_MESSAGE( "CPU SSE3: %d"
            , SDL_HasSSE3()
        );

        LOGGER_MESSAGE( "CPU SSE41: %d"
            , SDL_HasSSE41()
        );

        LOGGER_MESSAGE( "CPU SSE42: %d"
            , SDL_HasSSE42()
        );

        LOGGER_MESSAGE( "CPU AVX: %d"
            , SDL_HasAVX()
        );

        LOGGER_MESSAGE( "CPU AVX2: %d"
            , SDL_HasAVX2()
        );

        LOGGER_MESSAGE( "CPU AVX512F: %d"
            , SDL_HasAVX512F()
        );

        LOGGER_MESSAGE( "CPU NEON: %d"
            , SDL_HasNEON()
        );

        LOGGER_MESSAGE( "RAM: %d MB"
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

                m_accelerometer = joystick;
                break;
            }
        }

        if( m_accelerometer == nullptr )
        {
            LOGGER_MESSAGE_RELEASE( "Accelerometer not found"
            );
        }
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

        m_platformTags.clear();

        if( m_accelerometer != nullptr )
        {
            if( SDL_JoystickGetAttached( m_accelerometer ) )
            {
                SDL_JoystickClose( m_accelerometer );
            }

            m_accelerometer = nullptr;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDateTimeProviders );

        m_factoryDynamicLibraries = nullptr;
        m_factoryDateTimeProviders = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::runPlatform()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::updatePlatform()
    {
        m_prevTime = TIME_SYSTEM()
            ->getTimeMilliseconds();

        while( true )
        {
            uint64_t currentTime = TIME_SYSTEM()
                ->getTimeMilliseconds();

            float frameTime = (float)(currentTime - m_prevTime);

            for( TimerDesc & desc : m_timers )
            {
                if( desc.id == 0 )
                {
                    continue;
                }

                desc.time -= frameTime;

                if( desc.time > 0.f )
                {
                    continue;
                }

                desc.time += desc.milliseconds;

                desc.lambda();
            }

            m_prevTime = currentTime;

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

                SDL_Delay( 0 );
            }
            else
            {
                SDL_Delay( 100 );
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
            else
            {
                SDL_Delay( 100 );
            }

            APPLICATION_SERVICE()
                ->endUpdate();

#if defined(MENGINE_PLATFORM_WINDOWS)
            SDL_Delay( 1 );
#endif
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::openUrlInDefaultBrowser( const Char * _url )
    {
        MENGINE_UNUSED( _url );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::stopPlatform()
    {
        m_running = false;
        m_shouldQuit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SDLPlatform::addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc )
    {
        uint32_t new_id = ++m_enumerator;

        TimerDesc desc;
        desc.id = new_id;
        desc.milliseconds = _milliseconds;
        desc.time = _milliseconds;
        desc.lambda = _lambda;
        desc.doc = _doc;

        m_timers.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::removeTimer( uint32_t _id )
    {
        VectorTimers::iterator it_found = std::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_timers.end() );

        TimerDesc & desc = *it_found;

        desc.id = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDLPlatform::getTicks() const
    {
        Uint32 ticks = SDL_GetTicks();

        return (uint64_t)ticks;
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
    void SDLPlatform::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle[0] = '\0';

            return;
        }

        MENGINE_STRCPY( m_projectTitle, _projectTitle );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::getProjectTitle( Char * _projectTitle ) const
    {
        MENGINE_STRCPY( _projectTitle, m_projectTitle );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        LOGGER_MESSAGE( "create window size %u:%u fullscreen %d"
            , _resolution.getWidth()
            , _resolution.getHeight()
            , _fullscreen
        );

        if( this->createWindow_( _resolution, _fullscreen ) == false )
        {
            return false;
        }

        SDL_GLContext glContext = SDL_GL_CreateContext( m_window );

        if( glContext == nullptr )
        {
            SDL_DestroyWindow( m_window );
            m_window = nullptr;

            return false;
        }

        m_glContext = glContext;

        int dw;
        int dh;
        SDL_GL_GetDrawableSize( m_window, &dw, &dh );

        float dwf = (float)dw;
        float dhf = (float)dh;

        m_sdlInput->updateSurfaceResolution( dwf, dhf );

#if defined(MENGINE_PLATFORM_IOS)
        APPLICATION_SERVICE()
            ->changeWindowResolution( Resolution( dw, dh ) );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        APPLICATION_SERVICE()
            ->changeWindowResolution( Resolution( dw, dh ) );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::hasPlatformTag( const ConstString & _tag ) const
    {
        return m_platformTags.hasTag( _tag );
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
        LOGGER_INFO( "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        SDLDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, nullptr, "can't create dynamic library '%s'"
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
    bool SDLPlatform::getDesktopResolution( Resolution & _resolution ) const
    {
#if defined(MENGINE_PLATFORM_IOS)
        if( m_window == nullptr )
        {
            return false;
        }

        int dw;
        int dh;
        SDL_GL_GetDrawableSize( m_window, &dw, &dh );

        _resolution.setWidth( static_cast<uint32_t>(dw) );
        _resolution.setHeight( static_cast<uint32_t>(dh) );
#elif defined(MENGINE_PLATFORM_ANDROID)
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
            LOGGER_ERROR( "failed %s"
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

        int wndPosX = static_cast<int>(_pos.x * width);
        int wndPosY = static_cast<int>(_pos.y * height);

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
        MENGINE_UNUSED( _vsync );

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
    bool SDLPlatform::notifyCursorIconSetup( const ConstString & _name, const ContentInterface * _content, const MemoryInterfacePtr & _buffer )
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
            int status = mkdir( _fullpath, 0700 );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' alredy exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_OSX)
            int status = mkdir( _fullpath, 0700 );

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
            WChar unicode_fullpath[MENGINE_MAX_PATH];
            Helper::utf8ToUnicode( _fullpath, unicode_fullpath, MENGINE_MAX_PATH );

            BOOL successful = ::CreateDirectoryW( unicode_fullpath, NULL );

            if( successful == FALSE )
            {
                DWORD err = GetLastError();

                switch( err )
                {
                case ERROR_ALREADY_EXISTS:
                    {
                        LOGGER_WARNING( "'%s' alredy exists"
                            , _fullpath
                        );

                        return false;
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
    bool SDLPlatform::existDirectory( const Char * _path ) const
    {
        Char fullPath[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToA( fullPath, _path );

        Helper::pathRemoveFileSpecA( fullPath );

        size_t len = MENGINE_STRLEN( fullPath );

        if( len == 0 )	// current dir
        {
            return true;
        }

        if( fullPath[len - 1] == ':' )	// root dir
        {
            return true;	// let it be
        }

        bool exist = Detail::s_isDirectoryFullpath( fullPath );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectory( const Char * _path )
    {
        Char fullPath[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToA( fullPath, _path );

        Helper::pathRemoveFileSpecA( fullPath );

        if( Detail::s_isDirectoryFullpath( fullPath ) == true )
        {
            return true;
        }

        Helper::pathRemoveBackslashA( fullPath );

        VectorString paths;

        for( ;; )
        {
            paths.push_back( fullPath );

            if( Helper::pathRemoveFileSpecA( fullPath ) == false )
            {
                break;
            }

            Helper::pathRemoveBackslashA( fullPath );

            if( Detail::s_isDirectoryFullpath( fullPath ) == true )
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

            if( Detail::s_createDurectoryFullpath( path.c_str() ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::existFile( const Char * _path )
    {
        Char userPath[MENGINE_MAX_PATH];
        this->getUserPath( userPath );

        Char pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToA( pathCorrect, _path );

        Char fullPath[MENGINE_MAX_PATH];
        MENGINE_STRCPY( fullPath, userPath );
        MENGINE_STRCAT( fullPath, pathCorrect );

        struct stat sb;
        if( stat( fullPath, &sb ) == 0 && ((sb.st_mode) & S_IFMT) != S_IFDIR )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::removeFile( const Char * _path )
    {
        Char userPath[MENGINE_MAX_PATH];
        this->getUserPath( userPath );

        Char pathCorrect[MENGINE_MAX_PATH];
        Helper::pathCorrectBackslashToA( pathCorrect, _path );

        Char fullPath[MENGINE_MAX_PATH];
        MENGINE_STRCPY( fullPath, userPath );
        MENGINE_STRCAT( fullPath, pathCorrect );

        int result = ::remove( fullPath );

        if( result != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
    //////////////////////////////////////////////////////////////////////////
    static bool s_listDirectoryContents( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaFilePath & _lambda )
    {
        {
            WChar sPath[MENGINE_MAX_PATH];
            ::wsprintf( sPath, L"%s%s%s", _dir, _path, _mask );

            WIN32_FIND_DATA fdFile;
            HANDLE hFind = ::FindFirstFile( sPath, &fdFile );

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

                    WChar unicode_filepath[MENGINE_MAX_PATH];
                    ::wsprintf( unicode_filepath, L"%s%s", _path, fdFile.cFileName );

                    Char utf8_filepath[MENGINE_MAX_PATH];
                    if( Helper::unicodeToUtf8( unicode_filepath, utf8_filepath, MENGINE_MAX_PATH ) == false )
                    {
                        FindClose( hFind );

                        return false;
                    }

                    FilePath fp = Helper::stringizeFilePath( utf8_filepath );

                    _lambda( fp );

                } while( ::FindNextFile( hFind, &fdFile ) != FALSE );
            }

            ::FindClose( hFind );
        }

        {
            WChar sPath[MENGINE_MAX_PATH];
            ::wsprintf( sPath, L"%s%s*.*", _dir, _path );

            WIN32_FIND_DATA fdFile;
            HANDLE hFind = ::FindFirstFile( sPath, &fdFile );

            if( hFind == INVALID_HANDLE_VALUE )
            {
                return true;
            }

            do
            {
                if( ::wcscmp( fdFile.cFileName, L"." ) == 0
                    || ::wcscmp( fdFile.cFileName, L".." ) == 0 )
                {
                    continue;
                }

                if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                {
                    continue;
                }

                WChar nextPath[2048];
                ::wsprintf( nextPath, L"%s%s/", _path, fdFile.cFileName );

                s_listDirectoryContents( _dir, _mask, nextPath, _lambda );

            } while( ::FindNextFile( hFind, &fdFile ) != FALSE );

            ::FindClose( hFind );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        WChar unicode_base[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_mask[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WChar unicode_path[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        if( s_listDirectoryContents( unicode_base, unicode_mask, unicode_path, _lambda ) == false )
        {
            return false;
        }

        return true;
#else
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
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
#if defined(MENGINE_PLATFORM_WINDOWS)
        WChar unicode_filePath[MENGINE_MAX_PATH];
        Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH );

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

        time_t time = s_FileTimeToUnixTime( &write );

        ::CloseHandle( handle );

        return time;
#else
        return 0U;
#endif		
    }
    //////////////////////////////////////////////////////////////////////////
    DateTimeProviderInterfacePtr SDLPlatform::createDateTimeProvider( const DocumentPtr & _doc )
    {
        SDLDateTimeProviderPtr dateTimeProvider = m_factoryDateTimeProviders->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( dateTimeProvider, nullptr );

        return dateTimeProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::sleep( uint32_t _ms )
    {
        MENGINE_UNUSED( _ms );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::abort()
    {
        ::abort();
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer SDLPlatform::getPlatformExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_Window * SDLPlatform::getWindow() const
    {
        return m_window;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_GLContext SDLPlatform::getGLContext() const
    {
        return m_glContext;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SDLPlatform::addSDLEventHandler( const LambdaSDLEventHandler & _handler )
    {
        uint32_t id = GENERATE_UNIQUE_IDENTITY();

        SDLEventHandlerDesc desc;

        desc.id = id;
        desc.handler = _handler;

        m_sdlEventHandlers.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::removeSDLEventHandler( uint32_t _handlerId )
    {
        VectorSDLEventHandlers::iterator it_found = std::find_if( m_sdlEventHandlers.begin(), m_sdlEventHandlers.end(), [_handlerId]( const SDLEventHandlerDesc & _desc )
        {
            return _desc.id == _handlerId;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_sdlEventHandlers.end() );

        m_sdlEventHandlers.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
#if defined(MENGINE_PLATFORM_IOS)
        RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen );
#elif defined(MENGINE_PLATFORM_ANDROID)
        RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen );
#else

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
        //SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
        SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

        Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

#if defined(MENGINE_PLATFORM_IOS)
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;
        windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );

        //SDL_SetHint( SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight" );
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );

#elif defined(MENGINE_PLATFORM_ANDROID)
        windowFlags |= SDL_WINDOW_FULLSCREEN;

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );

        SDL_SetHint( SDL_HINT_ORIENTATIONS, "Portrait" );
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );

#else
        if( _fullscreen == true )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
#endif

#if defined(MENGINE_PLATFORM_IOS)
        SDL_SetHint( SDL_HINT_IOS_HIDE_HOME_INDICATOR, "1" );

        m_window = SDL_CreateWindow( m_projectTitle
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#elif defined(MENGINE_PLATFORM_ANDROID)
        m_window = SDL_CreateWindow( m_projectTitle
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

#else
        int width = static_cast<int>(_resolution.getWidth());
        int height = static_cast<int>(_resolution.getHeight());

        m_window = SDL_CreateWindow( m_projectTitle
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
            for( const SDLEventHandlerDesc & desc : m_sdlEventHandlers )
            {
                desc.handler( &sdlEvent );
            }

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
                    default:
                        break;
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
            default:
                break;
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
