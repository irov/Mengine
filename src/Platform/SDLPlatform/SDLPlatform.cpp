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

#   include <cstdio>
#   include <clocale>

#   include "Core/FileLogger.h"
#   include "Core/IniUtil.h"
#   include "Core/FilePath.h"

#   include "Factory/FactorableUnique.h"
#   include "Factory/FactoryDefault.h"

#   include "Logger/Logger.h"

//#	include "resource.h"

#   include <ctime>
#   include <algorithm>

#   include <sstream>
#   include <iomanip>

#   include <sys/stat.h>

#   define PARAM_UNUSED(x) ((void)x)

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Menge::SDLPlatform );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLPlatform::SDLPlatform()
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_sdlInput(nullptr)
        , m_icon(0)
        , m_shouldQuit(false)
        , m_running(false)
        , m_pause(false)
        , m_touchpad(true)
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
		DWORD len = (DWORD)::GetCurrentDirectory((DWORD)_len, _path);

		if (len == 0)
		{
			return 0;
		}

		_path[len] = L'/';
		_path[len + 1] = L'\0';

		Helper::pathCorrectBackslash( _path, _path );

		return (size_t)len + 1;
#else  
        _path[0] = L'\0';
        
        return 0;
#endif
    }
	//////////////////////////////////////////////////////////////////////////
	size_t SDLPlatform::getUserPath( WChar * _path, size_t _len ) const
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
		bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );

		FilePath userPath;
		if( (developmentMode == true && roamingMode == false) || noroamingMode == true )
		{
			WChar currentPathW[MENGINE_MAX_PATH];
			size_t currentPathLen = PLATFORM_SERVICE( m_serviceProvider )
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
			WString Project_Company = CONFIG_VALUE( m_serviceProvider, "Project", "Company", L"NONAME" );
			WString Project_Name = CONFIG_VALUE( m_serviceProvider, "Project", "Name", L"UNKNOWN" );

			String utf8_Project_Company;
			Helper::unicodeToUtf8( m_serviceProvider, Project_Company, utf8_Project_Company );

			String utf8_Project_Name;
			Helper::unicodeToUtf8( m_serviceProvider, Project_Name, utf8_Project_Name );

			char * sdl_prefPath = SDL_GetPrefPath( utf8_Project_Company.c_str(), utf8_Project_Name.c_str() );

			WString unicode_UserPath;
			Helper::utf8ToUnicode( m_serviceProvider, sdl_prefPath, unicode_UserPath );
			
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
        SDL_GetDisplayUsableBounds( 0, &rect );

        _resolution = Resolution((uint32_t)rect.w, (uint32_t)rect.h);
    }
	//////////////////////////////////////////////////////////////////////////
	static void MySDL_LogOutputFunction( void *userdata, int category, SDL_LogPriority priority, const char *message )
	{
		ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(userdata);

		EMessageLevel level;
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
		}

		size_t messageLen = strlen( message );

		LOGGER_SERVICE( serviceProvider )
			->logMessage( level, 0, message, messageLen );
	}
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::_initialize()
    {
        m_touchpad = false;

        if( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
        {
            LOGGER_CRITICAL(m_serviceProvider)("SDL initialization failed");
            return false;
        }

        const Char* sdlPlatform = SDL_GetPlatform();
        const int sdlRam = SDL_GetSystemRAM();

        m_platformName.addTag( Helper::stringizeString( m_serviceProvider, sdlPlatform ) );

        LOGGER_WARNING(m_serviceProvider)("Device info:"
            );

        LOGGER_WARNING(m_serviceProvider)("Platform: %s"
            , sdlPlatform
            );

        LOGGER_WARNING(m_serviceProvider)("RAM: %d MB"
            , sdlRam
            );

		SDL_LogSetOutputFunction( &MySDL_LogOutputFunction, m_serviceProvider );
		SDL_LogSetAllPriority( SDL_LOG_PRIORITY_WARN );

        
        m_sdlInput = new FactorableUnique<SDLInput>();

        m_sdlInput->setServiceProvider(m_serviceProvider);
        
        if( !m_sdlInput->initialize() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::update()
    {
        TIMER_SERVICE( m_serviceProvider )
            ->resetDeltaTime();

        while( true )
        {
            float frameTime = TIMER_SERVICE(m_serviceProvider)
                ->getDeltaTime();

            if( m_pause == true )
            {
                SDL_Delay( 100 );

                continue;
            }
            
            const bool quitRequest = this->processEvents();
            if( quitRequest )
            {
                break;
            }

            bool updating = APPLICATION_SERVICE( m_serviceProvider )
                ->beginUpdate();

            if( updating == true )
            {                
                APPLICATION_SERVICE( m_serviceProvider )
                    ->tick( frameTime );
            }
            else
            {
                SDL_Delay( 20 );
            }

            if( APPLICATION_SERVICE( m_serviceProvider )
                ->isFocus() == true )
            {
                if( APPLICATION_SERVICE( m_serviceProvider )
                    ->render() == true )
                {
                    APPLICATION_SERVICE( m_serviceProvider )
                        ->flush();
					
					SDL_GL_SwapWindow( m_window );
                }
            }

            APPLICATION_SERVICE( m_serviceProvider )
                ->endUpdate();

            SDL_Delay( 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::openUrlInDefaultBrowser( const WString & _url )
    {
        PARAM_UNUSED(_url);
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::_finalize()
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
    void SDLPlatform::stop()
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
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer SDLPlatform::getWindowHandle() const
    {
        return m_window;
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
        SDL_DisplayMode dm;
        if( SDL_GetDesktopDisplayMode( 0, &dm ) != 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLPlatform::getDesktopResolution failed %s"
                , SDL_GetError()
            );

            return false;
        }
            
        _resolution.setWidth(static_cast<uint32_t>(dm.w));
        _resolution.setHeight( static_cast<uint32_t>(dm.h) );
        
        return true;
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
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::hideKeyboard()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        PARAM_UNUSED(_resolution);
        PARAM_UNUSED(_fullscreen);

        // SDL can't toggle this without recreating the window!
        //m_windowResolution = _resolution;

        Uint32 flags = SDL_GetWindowFlags( m_window );
		
        if( _fullscreen == true && !(flags & SDL_WINDOW_FULLSCREEN) )
        {
            this->changeWindow_( _resolution, _fullscreen );
        }
        else if( _fullscreen == false && (flags & SDL_WINDOW_FULLSCREEN) )
        {
            this->changeWindow_( _resolution, _fullscreen );
        }
        //Uint32 flags = SDL_GetWindowFlags( m_window );

        //if( _fullscreen == true && !(flags & SDL_WINDOW_FULLSCREEN_DESKTOP) )
        //{
        //    this->changeWindow_( _resolution, _fullscreen );
        //}
        //else if( _fullscreen == false && (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) )
        //{
        //    this->changeWindow_( _resolution, _fullscreen );
        //}

        //this->createWindow( _resolution, _fullscreen );

        //const Resolution & contentResolution = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getContentResolution();

        //const Viewport & renderViewport = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getRenderViewport();

        //RENDER_SERVICE( m_serviceProvider )
        //    ->createRenderWindow( _resolution
        //        , contentResolution
        //        , renderViewport
        //        , 0
        //        , _fullscreen
        //        , 0
        //        , 0
        //        , 0
        //    );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyVsyncChanged( bool _vsync )
    {
        PARAM_UNUSED(_vsync);

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
            SDL_ShowCursor(SDL_ENABLE);
        }
        else
        {
            SDL_ShowCursor(SDL_DISABLE);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
    {
        PARAM_UNUSED(_name);
        PARAM_UNUSED(_path);
        PARAM_UNUSED(_buffer);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::onEvent( const ConstString & _event, const TMapWParams & _params )
    {
        PARAM_UNUSED(_event);
        PARAM_UNUSED(_params);
    }
	//////////////////////////////////////////////////////////////////////////
	static bool s_isDirectoryFullpath( ServiceProviderInterface * _serviceProvider, const WChar * _fullpath )
	{
		Char utf8_fullpath[MENGINE_MAX_PATH];

		size_t utf8_fullpathLen;
		UNICODE_SERVICE( _serviceProvider )
			->unicodeToUtf8( _fullpath, (size_t)-1, utf8_fullpath, MENGINE_MAX_PATH, &utf8_fullpathLen );

		struct stat sb;
		if( stat( utf8_fullpath, &sb ) == 0 && ((sb.st_mode)& S_IFMT) == S_IFDIR )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_createDurectoryFullpath( ServiceProviderInterface * _serviceProvider, const WChar * _fullpath )
	{
#	ifdef __MACH__
		Char utf8_fullpath[MENGINE_MAX_PATH];

		size_t utf8_fullpathLen;
		UNICODE_SERVICE( _serviceProvider )
			->unicodeToUtf8( _fullpath, (size_t)-1, utf8_fullpath, MENGINE_MAX_PATH, &utf8_fullpathLen );

		int status = mkdir( utf8_fullpath, 0700 );
		
		if( status != 0 )
		{
			LOGGER_WARNING( _serviceProvider )("SDLPlatform::createDirectory %ls alredy exists"
				, _fullpath
				);

			return false;
		}
#	endif

#	ifdef WIN32		
		BOOL successful = CreateDirectoryW( _fullpath, NULL );
		
		if( successful == FALSE )
		{
			DWORD err = GetLastError();

			switch( err )
			{
			case ERROR_ALREADY_EXISTS:
				{
					LOGGER_WARNING( _serviceProvider )("SDLPlatform::createDirectory %ls alredy exists"
						, _fullpath
						);

					return false;
				}break;
			case ERROR_PATH_NOT_FOUND:
				{
					LOGGER_WARNING( _serviceProvider )("SDLPlatform::createDirectory %ls not found"
						, _fullpath
						);

					return false;
				}break;
			default:
				{
					LOGGER_WARNING( _serviceProvider )("SDLPlatform::createDirectory %ls unknown error %d"
						, _fullpath
						, err
						);

					return false;
				}break;
			}

			return false;
		}
#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::existDirectory( const WString & _path ) const
	{
		WChar userPath[MENGINE_MAX_PATH];
		this->getUserPath( userPath, MENGINE_MAX_PATH );

		WChar pathCorrect[MENGINE_MAX_PATH];
		Helper::pathCorrectBackslash( pathCorrect, _path.c_str() );

		WChar fullPath[MENGINE_MAX_PATH];
		wcscpy( fullPath, userPath );
		wcscat( fullPath, pathCorrect );

		bool exist = s_isDirectoryFullpath( m_serviceProvider, fullPath );

		return exist;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLPlatform::createDirectory(const WString & _path)
	{
		WChar userPath[MENGINE_MAX_PATH];
		this->getUserPath( userPath, MENGINE_MAX_PATH );

		WChar pathCorrect[MENGINE_MAX_PATH];
		Helper::pathCorrectBackslash(pathCorrect, _path.c_str());

		WChar fullPath[MENGINE_MAX_PATH];
		wcscpy( fullPath, userPath );
		wcscat( fullPath, pathCorrect );

		Helper::pathRemoveBackslash( fullPath );

		if( s_isDirectoryFullpath( m_serviceProvider, fullPath ) == true )
		{
			return true;
		}

		TVectorWString paths;

		for( ;; )
		{
			paths.push_back( fullPath );

			if( Helper::pathRemoveFileSpec( fullPath ) == false )
			{
				break;
			}

			if( s_isDirectoryFullpath( m_serviceProvider, fullPath ) == true )
			{
				break;
			}
		}

		for (TVectorWString::reverse_iterator
			it = paths.rbegin(),
			it_end = paths.rend();
			it != it_end;
			++it)
		{
			const WString & path = *it;

			if( s_createDurectoryFullpath( m_serviceProvider, path.c_str() ) == false )
			{
				return false;
			}
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
#	endif

		return 0U;
	}
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        PARAM_UNUSED(_path);
        PARAM_UNUSED(_file);
        PARAM_UNUSED(_data);
        PARAM_UNUSED(_size);
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
    {
        PARAM_UNUSED(_path);
        PARAM_UNUSED(_file);
        PARAM_UNUSED(_data);
        PARAM_UNUSED(_size);
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity )
    {
        size_t folderSize = _folder.size();
        size_t dirSize = _fileName.size();

        size_t filePathSize = folderSize + dirSize;

        if( filePathSize >= MENGINE_MAX_PATH )
        {
            return false;
        }

        Char filePath[MENGINE_MAX_PATH];
        stdex::memorycopy( filePath, 0, _folder.c_str(), folderSize );
        stdex::memorycopy( filePath, folderSize, _fileName.c_str(), dirSize );

        filePath[filePathSize] = '\0';
        //filePathSize += 1; //Null

        if( UNICODE_SERVICE( m_serviceProvider )
            ->utf8ToUnicode( filePath, filePathSize, _filePath, _capacity, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        //RENDER_SERVICE( m_serviceProvider )
        //    ->destroyRenderWindow();

        //SDL_Window * old_window = m_window;
        //SDL_HideWindow( old_window );
        //SDL_GL_MakeCurrent( old_window, nullptr );

		RENDER_SERVICE( m_serviceProvider )
			->onWindowChangeFullscreenPrepare( _fullscreen );

        this->destroyWindow_();
        this->createWindow( _resolution, _fullscreen );

		RENDER_SERVICE( m_serviceProvider )
			->onWindowChangeFullscreen( _fullscreen );

        //SDL_GL_MakeCurrent( m_window, m_glContext );

        //if( old_window != nullptr )
        //{
            //SDL_DestroyWindow( old_window );
            //old_window = nullptr;
        //}

        //this->destroyWindow_();

        //const Resolution & contentResolution = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getContentResolution();

        //const Viewport & renderViewport = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getRenderViewport();

        //uint32_t bits = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getWindowBits();

        //uint32_t FSAAType = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getWindowFSAAType();

        //uint32_t FSAAQuality = APPLICATION_SERVICE( m_serviceProvider )
        //    ->getWindowFSAAQuality();

        //RENDER_SERVICE( m_serviceProvider )
        //    ->createRenderWindow( _resolution
        //        , contentResolution
        //        , renderViewport
        //        , bits
        //        , _fullscreen
        //        , FSAAType
        //        , FSAAQuality
        //    );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        Menge::Char utf8Title[1024] = {0};
        size_t utf8Size = 0;
        UNICODE_SERVICE( m_serviceProvider )
            ->unicodeToUtf8( m_projectTitle.c_str()
                , m_projectTitle.size()
                , utf8Title
                , sizeof( utf8Title ) - 1
                , &utf8Size
            );

        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
        SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

        Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        if( _fullscreen )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        int width = static_cast<int>(_resolution.getWidth());
        int height = static_cast<int>(_resolution.getHeight());

        m_window = SDL_CreateWindow( utf8Title
            , SDL_WINDOWPOS_CENTERED
            , SDL_WINDOWPOS_CENTERED
            , width
            , height
            , windowFlags
        );

        if( m_window == nullptr )
        {
            return false;
        }

        m_sdlInput->updateSurfaceResolution( static_cast<float>(width),
            static_cast<float>(height) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::destroyWindow_()
    {
        if( m_window != nullptr )
        {
            SDL_DestroyWindow( m_window );
            m_window = nullptr;
        }

		if( m_glContext != nullptr )
		{
			SDL_GL_DeleteContext( m_glContext );
			m_glContext = nullptr;
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
        while( SDL_PollEvent(&sdlEvent) != 0 && !quitRequest )
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
                                bool fullscreen = APPLICATION_SERVICE( m_serviceProvider )
                                    ->getFullscreenMode();

                                APPLICATION_SERVICE( m_serviceProvider )
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
                        {
                            break;
                        }

                    case SDL_WINDOWEVENT_CLOSE:
                        {
                            SDL_Event newEvent;
                            newEvent = sdlEvent;
                            newEvent.type = SDL_QUIT;

                            SDL_PushEvent( &newEvent );
                            break;
                        }
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
}
