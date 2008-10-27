/*
 *  MacOSApplication.cpp
 *  Mac
 *
 *  Created by Yuriy Levchenko on 22.10.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "Config/Config.h"

#include "MacOSApplication.h"
#include "OSXTimer.h"
#include "Menge/Application.h"

#include "Interface/LogSystemInterface.h"

const Menge::TCharA * config_file = "application.xml";

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
		if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_ERROR );

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_opengl.h"
namespace Menge
{
	MacOSApplication::MacOSApplication( const StringA& _commandLine )
		: m_commandLine( _commandLine )
		, m_menge( NULL )
		, m_logSystem( NULL )
		, m_running( true )
		, m_handleMouse( true )
		, m_timer( NULL )
	{
	}
	
	MacOSApplication::~MacOSApplication()
	{

		if( m_menge != NULL )
		{
			m_menge->onDestroy();
			delete m_menge;
			m_menge = NULL;
		}
		if( m_timer != NULL )
		{
			delete m_timer;
			m_timer = NULL;
		}	
	}
	
	bool MacOSApplication::start()
	{
		m_timer = new OSXTimer();

		m_menge = new Application( this );
		if( m_menge == NULL )
		{
			return false;
		}
		
		m_logSystem = m_menge->initializeLogSystem();
		
		m_logSystem->setVerboseLevel( Menge::LM_MAX );
		LOG( "Initializing Mengine..." );
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
	
	void MacOSApplication::loop()
	{
		float deltaTime = 0.0;
		int thisTime = 0;
		int lastTime = 0;

		while( m_running )
		{
			//’ј 
			SDL_Event event;

			while(SDL_PollEvent(&event))
			{
				switch(event.type) 
				{
				//case SDL_KEYDOWN:
				//	m_menge->onKeyEvent( _key_map[event.key.keysym.sym]/*event.key.keysym.sym*/, (char)event.key.keysym.unicode, true );
				//	break;

				//case SDL_KEYUP:
				//	m_menge->onKeyEvent( _key_map[event.key.keysym.sym]/*event.key.keysym.sym*/, (char)event.key.keysym.unicode, false );
				//	break;

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
					if( m_handleMouse == false )
					{
						break;
					}
					m_menge->onMouseMove( float(event.motion.x), float(event.motion.y), 0 );
					break;

				case SDL_MOUSEBUTTONDOWN:
					if( m_handleMouse == false )
					{
						break;
					}
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
					if( m_handleMouse == false )
					{
						break;
					}					
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

			thisTime = SDL_GetTicks();
			deltaTime = (float)(thisTime - lastTime);
			lastTime = thisTime; 

			m_menge->onUpdate( deltaTime );
		}
	}
	
	void MacOSApplication::stop()
	{
		m_running = false;
	}
	
	//////////////////////////////////////////////////////////////////////////
	TimerInterface * MacOSApplication::getTimer() const
	{
		return m_timer;
	}
	
	WindowHandle MacOSApplication::createWindow( const Menge::String& _name, std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		return 0;
	}
	
	std::size_t MacOSApplication::getDesktopWidth() const
	{
		return 0;
	}
	
	std::size_t MacOSApplication::getDesktopHeight() const
	{
		return 0;
	}
	
	void MacOSApplication::minimizeWindow()
	{
	
	}
	
	void MacOSApplication::setDesktopResolution( std::size_t _width, std::size_t _height )
	{
	}
	
	void MacOSApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
	}
	
	float MacOSApplication::getDeltaTime()
	{
		return 0.0f;
	}
	
	SystemDLLInterface* MacOSApplication::loadSystemDLL( const String& _dll )
	{
		return NULL;
	}
	
	void MacOSApplication::unloadSystemDLL( SystemDLLInterface* _interface )
	{
	}
	
	void MacOSApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
	}
	
	void MacOSApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
	}
}	// namespace Menge