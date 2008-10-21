#	include "Config/Config.h"

#	include "SDLApplication.h"
#	include "Menge/Application.h"
#	include "SystemDLL.h"
#	include "Interface/LogSystemInterface.h"

//#	include <strsafe.h>

#	include "resource.h"
#	include "Menge/Utils.h"

#	include "..\dependencies\ois\\includes\OIS.h"

const Menge::TCharA * config_file = "application.xml";
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	typedef std::map<SDLKey, OIS::KeyCode> InputKeyMap;
	InputKeyMap _key_map;
	//////////////////////////////////////////////////////////////////////////
	SDLApplication::SDLApplication( const StringA& _commandLine ) 
		: m_running( true )
		, m_frameTime( 0.f )
		, m_focus( true )
		, m_name( "Mengine" )
		, m_cursorInArea( false )
		, m_fullscreen( false )
		, m_handleMouse( true )
		, m_commandLine( _commandLine )
		, m_menge( NULL )
	{
		//ÕÀÊ.
		using namespace OIS;
		_key_map.insert(InputKeyMap::value_type(SDLK_ESCAPE,KC_ESCAPE));
		_key_map.insert(InputKeyMap::value_type(SDLK_1, KC_1));
		_key_map.insert(InputKeyMap::value_type(SDLK_2, KC_2));
		_key_map.insert(InputKeyMap::value_type(SDLK_3, KC_3));
		_key_map.insert(InputKeyMap::value_type(SDLK_4, KC_4));
		_key_map.insert(InputKeyMap::value_type(SDLK_5, KC_5));
		_key_map.insert(InputKeyMap::value_type(SDLK_6, KC_6));
		_key_map.insert(InputKeyMap::value_type(SDLK_7, KC_7));
		_key_map.insert(InputKeyMap::value_type(SDLK_8, KC_8));
		_key_map.insert(InputKeyMap::value_type(SDLK_9, KC_9));
		_key_map.insert(InputKeyMap::value_type(SDLK_0, KC_0));
		_key_map.insert(InputKeyMap::value_type(SDLK_MINUS, KC_MINUS));
		_key_map.insert(InputKeyMap::value_type(SDLK_EQUALS, KC_EQUALS));
		_key_map.insert(InputKeyMap::value_type(SDLK_BACKSPACE, KC_BACK));
		_key_map.insert(InputKeyMap::value_type(SDLK_TAB, KC_TAB));
		_key_map.insert(InputKeyMap::value_type(SDLK_q, KC_Q));
		_key_map.insert(InputKeyMap::value_type(SDLK_w, KC_W));
		_key_map.insert(InputKeyMap::value_type(SDLK_e, KC_E));
		_key_map.insert(InputKeyMap::value_type(SDLK_r, KC_R));
		_key_map.insert(InputKeyMap::value_type(SDLK_t, KC_T));
		_key_map.insert(InputKeyMap::value_type(SDLK_y, KC_Y));
		_key_map.insert(InputKeyMap::value_type(SDLK_u, KC_U));
		_key_map.insert(InputKeyMap::value_type(SDLK_i, KC_I));
		_key_map.insert(InputKeyMap::value_type(SDLK_o, KC_O));
		_key_map.insert(InputKeyMap::value_type(SDLK_p, KC_P));
		_key_map.insert(InputKeyMap::value_type(SDLK_RETURN, KC_RETURN));
		_key_map.insert(InputKeyMap::value_type(SDLK_LCTRL, KC_LCONTROL));
		_key_map.insert(InputKeyMap::value_type(SDLK_a, KC_A));
		_key_map.insert(InputKeyMap::value_type(SDLK_s, KC_S));
		_key_map.insert(InputKeyMap::value_type(SDLK_d, KC_D));
		_key_map.insert(InputKeyMap::value_type(SDLK_f, KC_F));
		_key_map.insert(InputKeyMap::value_type(SDLK_g, KC_G));
		_key_map.insert(InputKeyMap::value_type(SDLK_h, KC_H));
		_key_map.insert(InputKeyMap::value_type(SDLK_j, KC_J));
		_key_map.insert(InputKeyMap::value_type(SDLK_k, KC_K));
		_key_map.insert(InputKeyMap::value_type(SDLK_l, KC_L));
		_key_map.insert(InputKeyMap::value_type(SDLK_SEMICOLON, KC_SEMICOLON));
		_key_map.insert(InputKeyMap::value_type(SDLK_COLON, KC_COLON));
		_key_map.insert(InputKeyMap::value_type(SDLK_QUOTE, KC_APOSTROPHE));
		_key_map.insert(InputKeyMap::value_type(SDLK_BACKQUOTE, KC_GRAVE));
		_key_map.insert(InputKeyMap::value_type(SDLK_LSHIFT, KC_LSHIFT));
		_key_map.insert(InputKeyMap::value_type(SDLK_BACKSLASH, KC_BACKSLASH));
		_key_map.insert(InputKeyMap::value_type(SDLK_z, KC_Z));
		_key_map.insert(InputKeyMap::value_type(SDLK_x, KC_X));
		_key_map.insert(InputKeyMap::value_type(SDLK_c, KC_C));
		_key_map.insert(InputKeyMap::value_type(SDLK_v, KC_V));
		_key_map.insert(InputKeyMap::value_type(SDLK_b, KC_B));
		_key_map.insert(InputKeyMap::value_type(SDLK_n, KC_N));
		_key_map.insert(InputKeyMap::value_type(SDLK_m, KC_M));
		_key_map.insert(InputKeyMap::value_type(SDLK_COMMA, KC_COMMA));
		_key_map.insert(InputKeyMap::value_type(SDLK_PERIOD, KC_PERIOD));
		_key_map.insert(InputKeyMap::value_type(SDLK_RSHIFT, KC_RSHIFT));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_MULTIPLY, KC_MULTIPLY));
		_key_map.insert(InputKeyMap::value_type(SDLK_LALT, KC_LMENU));
		_key_map.insert(InputKeyMap::value_type(SDLK_SPACE, KC_SPACE));
		_key_map.insert(InputKeyMap::value_type(SDLK_CAPSLOCK, KC_CAPITAL));
		_key_map.insert(InputKeyMap::value_type(SDLK_F1, KC_F1));
		_key_map.insert(InputKeyMap::value_type(SDLK_F2, KC_F2));
		_key_map.insert(InputKeyMap::value_type(SDLK_F3, KC_F3));
		_key_map.insert(InputKeyMap::value_type(SDLK_F4, KC_F4));
		_key_map.insert(InputKeyMap::value_type(SDLK_F5, KC_F5));
		_key_map.insert(InputKeyMap::value_type(SDLK_F6, KC_F6));
		_key_map.insert(InputKeyMap::value_type(SDLK_F7, KC_F7));
		_key_map.insert(InputKeyMap::value_type(SDLK_F8, KC_F8));
		_key_map.insert(InputKeyMap::value_type(SDLK_F9, KC_F9));
		_key_map.insert(InputKeyMap::value_type(SDLK_F10, KC_F10));
		_key_map.insert(InputKeyMap::value_type(SDLK_NUMLOCK, KC_NUMLOCK));
		_key_map.insert(InputKeyMap::value_type(SDLK_SCROLLOCK, KC_SCROLL));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP7, KC_NUMPAD7));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP8, KC_NUMPAD8));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP9, KC_NUMPAD9));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_MINUS, KC_SUBTRACT));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP4, KC_NUMPAD4));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP5, KC_NUMPAD5));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP6, KC_NUMPAD6));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_PLUS, KC_ADD));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP1, KC_NUMPAD1));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP2, KC_NUMPAD2));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP3, KC_NUMPAD3));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP0, KC_NUMPAD0));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_PERIOD, KC_DECIMAL));
		_key_map.insert(InputKeyMap::value_type(SDLK_F11, KC_F11));
		_key_map.insert(InputKeyMap::value_type(SDLK_F12, KC_F12));
		_key_map.insert(InputKeyMap::value_type(SDLK_F13, KC_F13));
		_key_map.insert(InputKeyMap::value_type(SDLK_F14, KC_F14));
		_key_map.insert(InputKeyMap::value_type(SDLK_F15, KC_F15));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_EQUALS, KC_NUMPADEQUALS));
		_key_map.insert(InputKeyMap::value_type(SDLK_KP_DIVIDE, KC_DIVIDE));
		_key_map.insert(InputKeyMap::value_type(SDLK_SYSREQ, KC_SYSRQ));
		_key_map.insert(InputKeyMap::value_type(SDLK_RALT, KC_RMENU));
		_key_map.insert(InputKeyMap::value_type(SDLK_HOME, KC_HOME));
		_key_map.insert(InputKeyMap::value_type(SDLK_UP, KC_UP));
		_key_map.insert(InputKeyMap::value_type(SDLK_PAGEUP, KC_PGUP));
		_key_map.insert(InputKeyMap::value_type(SDLK_LEFT, KC_LEFT));
		_key_map.insert(InputKeyMap::value_type(SDLK_RIGHT, KC_RIGHT));
		_key_map.insert(InputKeyMap::value_type(SDLK_END, KC_END));
		_key_map.insert(InputKeyMap::value_type(SDLK_DOWN, KC_DOWN));
		_key_map.insert(InputKeyMap::value_type(SDLK_PAGEDOWN, KC_PGDOWN));
		_key_map.insert(InputKeyMap::value_type(SDLK_INSERT, KC_INSERT));
		_key_map.insert(InputKeyMap::value_type(SDLK_DELETE, KC_DELETE));
		_key_map.insert(InputKeyMap::value_type(SDLK_LSUPER, KC_LWIN));
		_key_map.insert(InputKeyMap::value_type(SDLK_RSUPER, KC_RWIN));
	}
	//////////////////////////////////////////////////////////////////////////
	SDLApplication::~SDLApplication()
	{
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

		// õç
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
		float deltaTime = 0.0;
		int thisTime = 0;
		int lastTime = 0;

		while( m_running )
		{

#if MENGE_PLATFORM == MENGE_PLATFORM_APPLE
			EventRef event = NULL;
			EventTargetRef targetWindow;
			targetWindow = GetEventDispatcherTarget();

			// If we are unable to get the target then we no longer care about events.
			if( !targetWindow ) return;

			// Grab the next event, process it if it is a window event
			if( ReceiveNextEvent( 0, NULL, kEventDurationNoWait, true, &event ) == noErr )
			{
				// Dispatch the event
				SendEventToEventTarget( event, targetWindow );
				ReleaseEvent( event );
			}
#elif MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS

			//ÕÀÊ
			SDL_Event event;

			while(SDL_PollEvent(&event))
			{
				switch(event.type) 
				{
				case SDL_KEYDOWN:
					m_menge->onKeyEvent( _key_map[event.key.keysym.sym]/*event.key.keysym.sym*/, (char)event.key.keysym.unicode, true );
					break;

				case SDL_KEYUP:
					m_menge->onKeyEvent( _key_map[event.key.keysym.sym]/*event.key.keysym.sym*/, (char)event.key.keysym.unicode, false );
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
#endif
			thisTime = SDL_GetTicks();
			deltaTime = (float)(thisTime - lastTime);
			lastTime = thisTime; 

			m_menge->onUpdate( deltaTime );
			
			//SDL_Delay(1);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::stop()
	{
		m_running = false;
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
		assert(0);
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		#if MENGE_PLATFORM == MENGE_PLATFORM_APPLE
				Str255 msg;
				c2pstrcpy(msg, _message.c_str());

				Str255 header;
				c2pstrcpy(header, _header.c_str());

				StandardAlert(kAlertStopAlert, (ConstStr255Param)header, (ConstStr255Param)msg, NULL, NULL);
		#endif
	}
	//////////////////////////////////////////////////////////////////////////
}