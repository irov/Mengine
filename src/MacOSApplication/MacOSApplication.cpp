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
#include <AGL/agl.h>
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
	std::string s_macBundlePath()
	{
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		assert(mainBundle);

		CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
		assert(mainBundleURL);

		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		assert(cfStringRef);

		CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingUTF8);

		CFRelease(mainBundleURL);
		CFRelease(cfStringRef);

		return std::string(path);
	}
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
		, m_fullscreenOverride( false )
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
		m_timer = new OSXTimer();

		m_menge = new Application( this );
		if( m_menge == NULL )
		{
			return false;
		}
		
		if( m_commandLine.find( "-debugwd" ) == StringA::npos )
		{
			chdir( s_macBundlePath().c_str() );
			chdir( "../" );
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

		// create the window rect in global coords
		const String& projectTitle = m_menge->getProjectTitle();
		String ansiTitle = utf8ToAnsi( projectTitle );
		bool hasWindowPanel = m_menge->getHasWindowPanel();\
		const Menge::Resolution& winRes = m_menge->getResolution();
		m_window = createWindow_( ansiTitle, winRes[0], winRes[1], hasWindowPanel );
            
		// Get our view
        HIViewFindByID( HIViewGetRoot( m_window ), kHIViewWindowContentID, &m_view );
			
		// Set up our UPP for Window Events
        EventTypeSpec eventSpecs[] = {
				{kEventClassCommand, kEventProcessCommand },
                {kEventClassWindow, kEventWindowActivated},
                {kEventClassWindow, kEventWindowDeactivated},
                {kEventClassWindow, kEventWindowShown},
                {kEventClassWindow, kEventWindowHidden},
                {kEventClassWindow, kEventWindowDragCompleted},
                {kEventClassWindow, kEventWindowBoundsChanged},
                {kEventClassWindow, kEventWindowExpanded},
                {kEventClassWindow, kEventWindowCollapsed},
                {kEventClassWindow, kEventWindowClosed},
				{ kEventClassMouse, kEventMouseMoved },
				{ kEventClassMouse, kEventMouseDown },
				{ kEventClassMouse, kEventMouseUp },						
				{ kEventClassMouse, kEventMouseDragged },	
				{ kEventClassMouse, kEventMouseWheelMoved }
	          };
			  
		/*EventTypeSpec clientSpecs[] = 
		{
			{ kEventClassControl, kEventControlTrackingAreaEntered },
			{ kEventClassControl, kEventControlTrackingAreaExited }
		};*/

		m_windowHandlerUPP = NewEventHandlerUPP( MacOSApplication::s_windowHandler );
            
        // Install the standard event handler for the window
        EventTargetRef target = GetWindowEventTarget(m_window);
		InstallStandardEventHandler(target);
            
 		/*HIViewTrackingAreaRef m_trackingRef;
		OSStatus err = HIViewNewTrackingArea( m_view, NULL, 0, &m_trackingRef );
		InstallControlEventHandler( m_view, s_clientHandler, GetEventTypeCount(clientSpecs), clientSpecs, (void*)this, NULL );*/
		
	    // We also need to install the WindowEvent Handler, we pass along the window with our requests
        InstallEventHandler( target, m_windowHandlerUPP, GetEventTypeCount(eventSpecs), eventSpecs, (void*)this, &m_windowEventHandler );
				
		// Display and select our window
		ShowWindow( m_window );
		SelectWindow( m_window );
		
		//HideCursor();
		Rect rect;
		Point point = { 0, 0 };
		GetWindowBounds( m_window, kWindowContentRgn, &rect);
		ShieldCursor( &rect, point );
		
		//CFStringRef ssss = CFStringCreateWithCString( NULL, "aaaaaaa", CFStringGetSystemEncoding() );
		//OSStatus stat = SetMenuTitleWithCFString( AcquireRootMenu(), ssss );
		//printf( "%d", stat );
			
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
	//////////////////////////////////////////////////////////////////////////
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
		if( m_menge->getFullscreenMode() == true )
		{
			m_menge->setFullscreenMode( false );
			m_fullscreenOverride = true;
		}
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
	WindowRef MacOSApplication::createWindow_( const String& _title, int _width, int _height, bool _hasWindowPanel )
	{
		WindowRef winRef = NULL;
		::Rect windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = _width;
		windowRect.bottom = _height;
	
		//OSStatus err = CreateNibReference( CFSTR("MacOSApplication"), &m_nibRef );
		//err = SetMenuBarFromNib( m_nibRef, CFSTR("MenuBar") );
		//MenuRef ref = GetMenuHandle( 0 );
		
		//err = SetMenuTitleWithCFString( ref, CFSTR("AAAAA") );
		

		// set the default attributes for the window
		WindowAttributes windowAttrs = kWindowHideOnFullScreenAttribute | kWindowStandardHandlerAttribute ;
		if( _hasWindowPanel )
		{
			windowAttrs |= kWindowCloseBoxAttribute
			| kWindowCollapseBoxAttribute
			| kWindowInWindowMenuAttribute;
		}
		else
		{
			windowAttrs |= kWindowNoTitleBarAttribute;
		}
			
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

		// Event class
		UInt32 eventClass = GetEventClass( event );
		UInt32 eventKind = GetEventKind( event );
		
		if( eventClass == kEventClassCommand )
		{
			switch( eventKind ) 
			{
				case kEventProcessCommand:
					HICommand command;
					GetEventParameter( event, kEventParamDirectObject, kEventParamHICommand, NULL, sizeof(command), NULL, &command ); // get command
					switch( command.commandID )
					{
						case kHICommandQuit:
							m_menge->onClose();
							break;
					}
					break;
			}
		}
		else if( eventClass == kEventClassWindow )
		{
			switch( eventKind )
			{	
				case kEventWindowExpanded:
					if( m_fullscreenOverride == true )
					{
						m_menge->setFullscreenMode( true );
						m_fullscreenOverride = false;
					}
					break;
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
					{
						Rect rect;
						Point point = { 0, 0 };
						GetWindowBounds( m_window, kWindowContentRgn, &rect);
						ShieldCursor( &rect, point );
					}
					break;
				case kEventWindowBoundsChanged:
					break;	
				default:
					status = eventNotHandledErr;
					break;
			}
		}
		else if( eventClass == kEventClassMouse )
		{
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
		
		}
		return status;
	}
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::s_clientHandler( EventHandlerCallRef nextHandler, EventRef event, void* wnd )
	{
		MacOSApplication* thisApp = (MacOSApplication*)wnd;
		return thisApp->clientHandler( nextHandler, event );
	}	
	//////////////////////////////////////////////////////////////////////////
	OSStatus MacOSApplication::clientHandler( EventHandlerCallRef nextHandler, EventRef event )
	{
		OSStatus status = noErr;

		/*// Event class
		UInt32 eventClass = GetEventClass( event );
		UInt32 eventKind = GetEventKind( event );
		
		if( eventClass == kEventClassControl && m_handleMouse )
		{
			if( eventKind == kEventControlTrackingAreaEntered )
			{
				HideCursor();
			}
			else if( eventKind == kEventControlTrackingAreaExited )
			{
				ShowCursor();
			}
		}*/
		
		return status;
	}
	//////////////////////////////////////////////////////////////////////////
	String MacOSApplication::ansiToUtf8( const String& _ansi )
	{
		OSStatus err = noErr;
		TECObjectRef tec = 0;
	    ByteCount bytesConsumed = 0;
		ByteCount bytesProduced = 0;

	    TextEncoding inputEncoding	= CreateTextEncoding( CFStringGetSystemEncoding(),
                                        kTextEncodingDefaultVariant,
                                        kTextEncodingDefaultFormat);

		TextEncoding outputEncoding = CreateTextEncoding( kTextEncodingUnicodeDefault,
                                        kTextEncodingDefaultVariant,
                                        kUnicodeUTF8Format);

		err = TECCreateConverter( &tec, inputEncoding, outputEncoding );

		std::size_t bufLen = _ansi.length() * 4;
		String out;
		char* buffer = new char[bufLen];
		if (err == noErr)
		{
			err = TECConvertText(tec,
                    (ConstTextPtr) _ansi.c_str(),
                    _ansi.length(),				// inputBufferLength
                    &bytesConsumed,				// actualInputLength
                    (TextPtr) buffer,			// outputBuffer
                    bufLen,					// outputBufferLength
                    &bytesProduced);			// actualOutputLength
			
			out.assign( buffer, bytesProduced );

			TECFlushText( tec, (TextPtr) buffer, bufLen, &bytesProduced );
			
			if( bytesProduced > 0 )
			{
				out += String( buffer, bytesProduced );
			}
			TECDisposeConverter(tec);
		}
		delete[] buffer;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	String MacOSApplication::utf8ToAnsi( const String& _utf8 )
	{
		OSStatus err = noErr;
		TECObjectRef tec = 0;
	    ByteCount bytesConsumed = 0;
		ByteCount bytesProduced = 0;

	    TextEncoding outputEncoding	= CreateTextEncoding( CFStringGetSystemEncoding(),
                                        kTextEncodingDefaultVariant,
                                        kTextEncodingDefaultFormat);

		TextEncoding inputEncoding = CreateTextEncoding( kTextEncodingUnicodeDefault,
                                        kTextEncodingDefaultVariant,
                                        kUnicodeUTF8Format);

		err = TECCreateConverter( &tec, inputEncoding, outputEncoding );

		std::size_t bufLen = _utf8.length();
		String out;
		char* buffer = new char[bufLen];
		if (err == noErr)
		{
			err = TECConvertText(tec,
                    (ConstTextPtr) _utf8.c_str(),
                    _utf8.length(),				// inputBufferLength
                    &bytesConsumed,				// actualInputLength
                    (TextPtr) buffer,			// outputBuffer
                    bufLen,					// outputBufferLength
                    &bytesProduced);			// actualOutputLength

			out.assign( buffer, bytesProduced );
			
			TECFlushText( tec, (TextPtr) buffer, bufLen, &bytesProduced );
			if( bytesProduced > 0 )
			{
				out += String( buffer, bytesProduced );
			}
			TECDisposeConverter(tec);
		}
		
		delete[] buffer;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge