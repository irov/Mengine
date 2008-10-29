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
#include "LoggerConsole.h"
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
//#include "SDL/include/SDL_opengl.h"
namespace Menge
{
	MacOSApplication::MacOSApplication( const StringA& _commandLine )
		: m_commandLine( _commandLine )
		, m_menge( NULL )
		, m_logSystem( NULL )
		, m_running( true )
		, m_handleMouse( true )
		, m_timer( NULL )
		, m_loggerConsole( NULL )
		, m_window( NULL )
		, m_view( NULL )
		, m_eventHandler( NULL )
	{
	}
	
	MacOSApplication::~MacOSApplication()
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
		
		if( m_logSystem != NULL && m_commandLine.find( "-console" ) != StringA::npos )
		{
			m_loggerConsole = new LoggerConsole();
			m_logSystem->registerLogger( m_loggerConsole );

			LOG_ERROR( "LogSystem initialized successfully" );	// log message anyway
		}

		if( m_logSystem != NULL && m_commandLine.find( "-verbose" ) != StringA::npos )
		{
			m_logSystem->setVerboseLevel( LM_MAX );

			LOG( "Verbose logging mode enabled" );
		}	
		
		LOG( "Initializing Mengine..." );
		if( m_menge->initialize( config_file, m_commandLine.c_str(), true ) == false )
		{
			return false;
		}
		
		// TODO if fullscreen
		// {
		// }
		// else
					// create the window rect in global coords
		const String& projectTitle = m_menge->getProjectTitle();
		m_window = createWindow_( projectTitle, 1024, 768 );
            
		// Get our view
        HIViewFindByID( HIViewGetRoot( m_window ), kHIViewWindowContentID, &m_view );
			
		// Set up our UPP for Window Events
        EventTypeSpec eventSpecs[] = {
                {kEventClassWindow, kEventWindowActivated},
                {kEventClassWindow, kEventWindowDeactivated},
                {kEventClassWindow, kEventWindowShown},
                {kEventClassWindow, kEventWindowHidden},
                {kEventClassWindow, kEventWindowDragCompleted},
                {kEventClassWindow, kEventWindowBoundsChanged},
                {kEventClassWindow, kEventWindowExpanded},
                {kEventClassWindow, kEventWindowCollapsed},
                {kEventClassWindow, kEventWindowClosed}
            };
            
		EventHandlerUPP handlerUPP = NewEventHandlerUPP( MacOSApplication::s_windowHandler );
            
        // Install the standard event handler for the window
        EventTargetRef target = GetWindowEventTarget(m_window);
		InstallStandardEventHandler(target);
            
        // We also need to install the WindowEvent Handler, we pass along the window with our requests
        InstallEventHandler( target, handlerUPP, 9, eventSpecs, (void*)this, &m_eventHandler );
			
		// Display and select our window
		ShowWindow( m_window );
		SelectWindow( m_window );
			
		if( m_menge->createRenderWindow( (WindowHandle)m_window ) == false )
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
			/*SDL_Event event;

			while(SDL_PollEvent(&event))
			{
				switch(event.type) 
				{
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
			}*/
			// OSX Message Pump
			EventRef event = NULL;
			EventTargetRef targetWindow;
			targetWindow = GetEventDispatcherTarget();
    
			// Grab the next event, process it if it is a window event
			if( targetWindow && ReceiveNextEvent( 0, NULL, kEventDurationNoWait, true, &event ) == noErr )
			{
				// Dispatch the event
				SendEventToEventTarget( event, targetWindow );
				ReleaseEvent( event );
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
	
	WindowRef MacOSApplication::createWindow_( const String& _title, int _width, int _height )
	{
		WindowRef winRef = NULL;
		::Rect windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = _width;
		windowRect.bottom = _height;
			
		// set the default attributes for the window
		WindowAttributes windowAttrs = kWindowStandardDocumentAttributes
			| kWindowStandardHandlerAttribute 
			| kWindowInWindowMenuAttribute
			| kWindowHideOnFullScreenAttribute;
			
		// Create the window
		CreateNewWindow( kDocumentWindowClass, windowAttrs, &windowRect, &winRef );
			
		// Color the window background black
		SetThemeWindowBackground(winRef, kThemeBrushBlack, true);
			
		// Set the title of our window
		CFStringRef titleRef = CFStringCreateWithCString( kCFAllocatorDefault, _title.c_str(), CFStringGetSystemEncoding() );
		SetWindowTitleWithCFString( winRef, titleRef );
			
		// Center our window on the screen
		RepositionWindow( winRef, NULL, kWindowCenterOnMainScreen );
            
		return winRef;
	}
	
	OSStatus MacOSApplication::s_windowHandler( EventHandlerCallRef nextHandler, EventRef event, void* wnd )
	{
		MacOSApplication* thisApp = (MacOSApplication*)wnd;
		return thisApp->windowHandler( nextHandler, event );
	}
	
	OSStatus MacOSApplication::windowHandler( EventHandlerCallRef nextHandler, EventRef event )
	{
		OSStatus status = noErr;

		// We only get called if a window event happens
		UInt32 eventKind = GetEventKind( event );
		
	    switch( eventKind )
		{	
	        case kEventWindowActivated:
				m_menge->onFocus( true );
				break;
			case kEventWindowDeactivated:
				m_menge->onFocus( false );
				break;
			case kEventWindowClosed:
				m_menge->onClose();
				break;
			case kEventWindowShown:
			case kEventWindowExpanded:
	        case kEventWindowHidden:
			case kEventWindowCollapsed:
			case kEventWindowDragCompleted:
			case kEventWindowBoundsChanged:
				break;	
			default:
				status = eventNotHandledErr;
				break;
		}
		return status;
	}
}	// namespace Menge