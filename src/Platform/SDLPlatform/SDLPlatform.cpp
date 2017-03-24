#   include "Config/Config.h"

#   include "SDLPlatform.h"

#   include "Interface/LoggerInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/OptionsInterface.h"
#   include "Interface/TimerInterface.h"

#   include <cstdio>
#   include <clocale>

#   include "Core/FileLogger.h"
#   include "Core/IniUtil.h"

#   include "Factory/FactorableUnique.h"
#   include "Factory/FactoryDefault.h"

#   include "Logger/Logger.h"

//#	include "resource.h"

#   include <ctime>
#   include <algorithm>

#   include <sstream>
#   include <iomanip>

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
        , m_width(0)
        , m_height(0)
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
        if( _len > 0 )
        {
            wcscpy( _path, L"" );
        }

        return 0;
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
        SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(m_window));
        SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(m_glContext));
        SDL_Quit();

        if( m_sdlInput != nullptr )
        {
            m_sdlInput->finalize();
            m_sdlInput = nullptr;
        }

        m_window = nullptr;
        m_glContext = nullptr;

        m_platformName.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::stop()
    {
        m_running = false;
        m_shouldQuit = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPlatform::createWindow( uint32_t _icon, const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen )
    {
        PARAM_UNUSED(_icon);
        PARAM_UNUSED(_fullscreen);

        Menge::Char utf8Title[1024] = { 0 };
        size_t utf8Size = 0;
        UNICODE_SERVICE( m_serviceProvider )->unicodeToUtf8(_projectTitle.c_str(), _projectTitle.size(),
                                                            utf8Title, sizeof(utf8Title) - 1, &utf8Size);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

        Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        if( _fullscreen )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }

        m_width = static_cast<int>(_resolution.getWidth());
        m_height = static_cast<int>(_resolution.getHeight());

        m_window = reinterpret_cast<WindowHandle>(SDL_CreateWindow(utf8Title,
                                                                   SDL_WINDOWPOS_CENTERED,
                                                                   SDL_WINDOWPOS_CENTERED,
                                                                   m_width, m_height,
                                                                   windowFlags));
        if( m_window == nullptr )
        {
            return false;
        }

        m_glContext = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(m_window));

        if( m_glContext == nullptr )
        {
            SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(m_window));
            return false;
        }

        m_sdlInput->updateSurfaceResolution(static_cast<float>(m_width),
                                            static_cast<float>(m_height));

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    WindowHandle SDLPlatform::getWindowHandle() const
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
    void SDLPlatform::getDesktopResolution( Resolution & _resolution ) const
    {
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
        {
            _resolution.setWidth(static_cast<uint32_t>(dm.w));
            _resolution.setHeight(static_cast<uint32_t>(dm.h));
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::minimizeWindow()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::setCursorPosition( const mt::vec2f & _pos )
    {
        const int wndPosX = static_cast<int>(_pos.x * m_width);
        const int wndPosY = static_cast<int>(_pos.y * m_width);
        // ! This function generates a mouse motion event !
        SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(m_window), wndPosX, wndPosY);
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
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPlatform::notifyVsyncChanged( bool _vsync )
    {
        PARAM_UNUSED(_vsync);
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
    void SDLPlatform::onEvent( const ConstString & _event, const TMapParams & _params )
    {
        PARAM_UNUSED(_event);
        PARAM_UNUSED(_params);
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
    bool SDLPlatform::processEvents()
    {
        bool quitRequest = m_shouldQuit;

        if( !quitRequest )
        {
            SDL_Event sdlEvent;
            while( SDL_PollEvent(&sdlEvent) != 0 && !quitRequest )
            {
                m_sdlInput->handleEvent(sdlEvent);

                switch( sdlEvent.type )
                {
                    case SDL_QUIT:
                        quitRequest = true;
                        break;
                }
            }
        }

        return quitRequest;
    }
}
