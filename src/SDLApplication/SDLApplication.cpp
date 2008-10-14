#	include "Config/Config.h"

#	include "SDLApplication.h"
#	include "Menge/Application.h"
#	include "SystemDLL.h"
#	include "LoggerConsole.h"
#	include "Interface/LogSystemInterface.h"

#	include <strsafe.h>

#	include "resource.h"
#	include "Menge/Utils.h"

const Menge::TCharA * config_file = "application.xml";
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SDLApplication::SDLApplication( const StringA& _commandLine ) 
		: m_running( true )
		, m_frameTime( 0.f )
		, m_focus( true )
		, m_name( "Mengine" )
		, m_cursorInArea( false )
		, m_fullscreen( false )
		, m_handleMouse( true )
		, m_loggerConsole( NULL )
		, m_commandLine( _commandLine )
		, m_menge( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SDLApplication::~SDLApplication()
	{
		if( m_logSystem != NULL && m_loggerConsole != NULL )
		{
			m_logSystem->unregisterLogger( m_loggerConsole );
			delete m_loggerConsole;
			m_loggerConsole = NULL;
		}

		if( m_menge != NULL )
		{
			m_menge->onDestroy();
			delete m_menge;
			m_menge = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::setDesktopResolution( std::size_t _width, std::size_t _height )
	{
		m_desktopWidth = _width;
		m_desktopHeight = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::start()
	{
		m_menge = new Application( this );

		if( m_menge == NULL )
		{
			return false;
		}


		m_logSystem = m_menge->initializeLogSystem();

		if( m_logSystem != NULL && m_commandLine.find( "-console" ) != StringA::npos )
		{
			m_loggerConsole = new LoggerConsole();
			m_logSystem->registerLogger( m_loggerConsole );
		}

		setDesktopResolution(1024,768);

		if( m_menge->initialize( config_file, m_commandLine.c_str(), true ) == false )
		{
			return false;
		}

		if( m_menge->createRenderWindow( 0 ) == false )
		{
			return false;
		}

		if( m_menge->initGame() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::loop()
	{
		LARGE_INTEGER time;
		::QueryPerformanceCounter(&m_lastTime);

		if( !::QueryPerformanceFrequency( &m_timerFrequency ) )
		{
			return;
		}

		::QueryPerformanceCounter(&m_timer);

		while( m_running )
		{
/*			SDL_Event event;

			while(SDL_PollEvent(&event))
			{
				printf("%d \n",event.type);
				switch(event.type) 
				{
				case SDL_KEYDOWN:
					m_menge->onKeyEvent( event.key.keysym.sym, (char)event.key.keysym.unicode, true );
					break;

				case SDL_KEYUP:
					m_menge->onKeyEvent( event.key.keysym.sym, (char)event.key.keysym.unicode, false );
					break;

				case SDL_ACTIVEEVENT:

					if(event.active.state & SDL_APPACTIVE)
					{
						m_menge->onFocus( false );
					}

					if(event.active.state & SDL_APPINPUTFOCUS)
					{
						m_menge->onFocus( true );
					}
					
					break;

				case SDL_VIDEORESIZE:
					m_menge->onWindowMovedOrResized();
					break;
					
				case SDL_QUIT:
					m_menge->onClose();
					break;

				case SDL_MOUSEMOTION:

					m_menge->onMouseMove( float(event.motion.x), float(event.motion.y), 0 );
					break;

				case SDL_MOUSEBUTTONDOWN:

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						m_menge->onMouseButtonEvent(0,true);					
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						m_menge->onMouseButtonEvent(1,true);										
					}
					else if(event.button.button == SDL_BUTTON_MIDDLE)
					{
						m_menge->onMouseButtonEvent(2,true);					
					}
					break;

				case SDL_MOUSEBUTTONUP:

					if(event.button.button == SDL_BUTTON_LEFT)
					{
						m_menge->onMouseButtonEvent(0,false);					
					}
					else if(event.button.button == SDL_BUTTON_RIGHT)
					{
						m_menge->onMouseButtonEvent(1,false);										
					}
					else if(event.button.button == SDL_BUTTON_MIDDLE)
					{
						m_menge->onMouseButtonEvent(2,false);					
					}
					break;

				default:
					break;
				}
			}
*/
			::QueryPerformanceCounter(&time);
			m_frameTime = static_cast<float>( time.QuadPart - m_lastTime.QuadPart ) / m_timerFrequency.QuadPart * 1000.0f;
			m_lastTime = time;

			m_menge->onUpdate( m_frameTime );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::createWindow(WindowHandle _handle)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle SDLApplication::createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t SDLApplication::getDesktopWidth() const
	{
		return m_desktopWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t SDLApplication::getDesktopHeight() const
	{
		return m_desktopHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::minimizeWindow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLLInterface* SDLApplication::loadSystemDLL( const String& _dll )
	{
		return new WinSystemDLL( _dll );
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::unloadSystemDLL( SystemDLLInterface* _interface )
	{
		delete static_cast<WinSystemDLL*>( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	float SDLApplication::getDeltaTime()
	{
		LARGE_INTEGER time;
		::QueryPerformanceCounter( &time );
		float deltaTime = static_cast<float>( time.QuadPart - m_timer.QuadPart )  / m_timerFrequency.QuadPart * 1000.0f;
		m_timer = time;

		return deltaTime;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}