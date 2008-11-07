/*
 *  MacOSApplication.cpp
 *  Mac
 *
 *  Created by Berserk on 22.10.08.
 *  Copyright 2008 __Menge__. All rights reserved.
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

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
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
		, m_windowHandlerUPP( NULL )
		, m_windowEventHandler( NULL )
		, m_mouseHandlerUPP( NULL )
		, m_mouseEventHandler( NULL )

	{
	}
	//////////////////////////////////////////////////////////////////////////
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
		
		if( m_mouseEventHandler != NULL )
		{
			RemoveEventHandler( m_mouseEventHandler );
		}
		if( m_mouseHandlerUPP != NULL )
		{
			DisposeEventHandlerUPP( m_mouseHandlerUPP );
		}
		if( m_windowEventHandler != NULL )
		{
			RemoveEventHandler( m_windowEventHandler );
		}
		if( m_windowHandlerUPP != NULL )
		{
			DisposeEventHandlerUPP( m_windowHandlerUPP );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	bool MacOSApplication::start()
	{
		//char clocale[100];
		//CFStringRef locale = CFLocaleGetIdentifier( CFLocaleCopyCurrent()/*CFLocaleGetSystem()*/ );
		//CFIndex inx = CFStringGetLength( locale );
		//bool err = CFStringGetFileSystemRepresentation( locale, clocale, 100 );
		//CFStringRef locale = CFLocaleCreateCanonicalLocaleIdentifierFromString( kCFAllocatorDefault, CFSTR( "English" ) );
		//bool err = CFStringGetFileSystemRepresentation( locale, clocale, 100 );
		//const char* loc = setlocale( LC_CTYPE, "POSIX" );
			
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
		
		m_desktopResolution[0] = CGDisplayPixelsWide( CGMainDisplayID() );
		m_desktopResolution[1] = CGDisplayPixelsHigh( CGMainDisplayID() );
		
		m_menge->setDesktopResolution(	Resolution( m_desktopResolution[0], m_desktopResolution[1] ) );
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
                {kEventClassWindow, kEventWindowClosed},
            };

		EventTypeSpec mouseSpecs[] = {
						{ kEventClassMouse, kEventMouseMoved },
						{ kEventClassMouse, kEventMouseDown },
						{ kEventClassMouse, kEventMouseUp },						
						{ kEventClassMouse, kEventMouseDragged },	
						{ kEventClassMouse, kEventMouseWheelMoved }					
		};
		
		m_windowHandlerUPP = NewEventHandlerUPP( MacOSApplication::s_windowHandler );
            
        // Install the standard event handler for the window
        EventTargetRef target = GetWindowEventTarget(m_window);
		InstallStandardEventHandler(target);
            
        // We also need to install the WindowEvent Handler, we pass along the window with our requests
        InstallEventHandler( target, m_windowHandlerUPP, 9, eventSpecs, (void*)this, &m_windowEventHandler );
		
		m_mouseHandlerUPP = NewEventHandlerUPP( MacOSApplication::s_mouseHandler );
        InstallEventHandler( target, m_mouseHandlerUPP, 4, mouseSpecs, (void*)this, &m_mouseEventHandler );
		//CGAssociateMouseAndMouseCursorPosition(false);
		
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
		long lastTime = 0;

		while( m_running )
		{
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

			thisTime = m_timer->getMilliseconds();
			deltaTime = (float)(thisTime - lastTime);
			lastTime = thisTime; 

			m_menge->onUpdate( deltaTime );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	TimerInterface * MacOSApplication::getTimer() const
	{
		return m_timer;
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle MacOSApplication::createWindow( const Menge::String& _name, std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MacOSApplication::getDesktopWidth() const
	{
		return m_desktopResolution[0];
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t MacOSApplication::getDesktopHeight() const
	{
		return m_desktopResolution[1];
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::minimizeWindow()
	{
		CollapseWindow( m_window, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	float MacOSApplication::getDeltaTime()
	{
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLLInterface* MacOSApplication::loadSystemDLL( const String& _dll )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::unloadSystemDLL( SystemDLLInterface* _interface )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void MacOSApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		CFStringRef headerRef = CFStringCreateWithCString( kCFAllocatorDefault, _header.c_str(), CFStringGetSystemEncoding() );
		CFStringRef messageRef = CFStringCreateWithCString( kCFAllocatorDefault, _message.c_str(), CFStringGetSystemEncoding() );
		DialogRef msgBoxRef = NULL;
		CreateStandardAlert( kAlertStopAlert, headerRef, messageRef, NULL, &msgBoxRef );
		DialogItemIndex item = 0;
		RunStandardAlert( msgBoxRef, NULL, &item );
		CFRelease( headerRef );
		CFRelease( messageRef );
	}
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::s_windowHandler( EventHandlerCallRef nextHandler, EventRef event, void* wnd )
	{
		MacOSApplication* thisApp = (MacOSApplication*)wnd;
		return thisApp->windowHandler( nextHandler, event );
	}
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::windowHandler( EventHandlerCallRef nextHandler, EventRef event )
	{
		OSStatus status = noErr;

		// We only get called if a window event happens
		UInt32 eventKind = GetEventKind( event );
		
	    switch( eventKind )
		{	
			case kEventWindowExpanded:
	        case kEventWindowActivated:
				m_menge->onFocus( true );
				break;
			case kEventWindowDeactivated:
			case kEventWindowCollapsed:
				m_menge->onFocus( false );
				break;
			case kEventWindowClosed:
				m_menge->onClose();
				break;
			case kEventWindowShown:
	        case kEventWindowHidden:
			case kEventWindowDragCompleted:
			case kEventWindowBoundsChanged:
				break;	
			default:
				status = eventNotHandledErr;
				break;
		}
		return status;
	}
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::s_mouseHandler( EventHandlerCallRef nextHandler, EventRef event, void* wnd )
	{
		MacOSApplication* thisApp = (MacOSApplication*)wnd;
		return thisApp->mouseHandler( nextHandler, event );
	}
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::mouseHandler( EventHandlerCallRef nextHandler, EventRef event )
	{
		OSStatus status = noErr;

		// We only get called if a window event happens
		UInt32 eventKind = GetEventKind( event );
		
	    switch( eventKind )
		{	
			case kEventMouseDragged:
			case kEventMouseMoved:
				{
					if( m_handleMouse == true )
					{
						HIPoint delta = {0.0f, 0.0f};
						GetEventParameter(event, kEventParamWindowMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);
						m_menge->onMouseMove(delta.x,delta.y,0.0f);
					}
					status = CallNextEventHandler( nextHandler, event );					
				}
				break;
			case kEventMouseDown:
				{
					if( m_handleMouse == true )
					{				
						EventMouseButton button = 0;
						int mouseButton = 3;
						UInt32 modifiers = 0;
			
						// Capture parameters
						GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
						GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
			
						if((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
						{
							mouseButton = 2;
						}
						else if((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
						{	
							mouseButton = 1;
						}
						else if(button == kEventMouseButtonPrimary)
						{
							mouseButton = 0;
						}
						m_menge->onMouseButtonEvent( mouseButton, true );
					}
					status = CallNextEventHandler( nextHandler, event );
				}
				break;
			case kEventMouseUp:
				{
					if( m_handleMouse == true )
					{
						EventMouseButton button = 0;
						int mouseButton = 3;
						UInt32 modifiers = 0;
			
						// Capture parameters
						GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
						GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
			
						if((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
						{
							mouseButton = 2;
						}
						else if((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
						{	
							mouseButton = 1;
						}
						else if(button == kEventMouseButtonPrimary)
						{
							mouseButton = 0;
						}
						m_menge->onMouseButtonEvent( mouseButton, false );
					}
					status = CallNextEventHandler( nextHandler, event );
				}
				break;
			case kEventMouseWheelMoved:
				{
					if( m_handleMouse == true )
					{
						SInt32 wheelDelta = 0;
						EventMouseWheelAxis	wheelAxis = 0; 

						// Capture parameters
						GetEventParameter( event, kEventParamMouseWheelAxis, typeMouseWheelAxis, NULL, sizeof(EventMouseWheelAxis), NULL, &wheelAxis );
						GetEventParameter( event, kEventParamMouseWheelDelta, typeSInt32, NULL, sizeof(SInt32), NULL, &wheelDelta );
			
						// If the Y axis of the wheel changed, then update the Z
						// Does OIS care about the X wheel axis?
						if( wheelAxis == kEventMouseWheelAxisY )
						{
							m_menge->onMouseMove( 0.0f, 0.0f, wheelDelta * 60 );
						}
					}
					status = CallNextEventHandler( nextHandler, event );
				}
				break;
	
			default:
				status = eventNotHandledErr;
				break;		
		}
		
		return status;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge