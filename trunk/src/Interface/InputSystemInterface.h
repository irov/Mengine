#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Math/vec2.h"

#	include "Core/Resolution.h"
#	include "Core/Viewport.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define MENGINE_INPUT_MAX_TOUCH 16
	//////////////////////////////////////////////////////////////////////////
	enum KeyCode
	{
		KC_UNASSIGNED = 0x00,
		KC_LBUTTON = 0x01,
		KC_RBUTTON = 0x02,
		KC_CANCEL = 0x03,
		KC_MBUTTON = 0x04,
		KC_XBUTTON1 = 0x05,
		KC_XBUTTON2 = 0x06,
		//undefined    = 0x07,
		KC_BACK = 0x08,
		KC_TAB = 0x09,
		//reserved     = 0x0A,
		//reserved     = 0x0B,
		KC_CLEAR = 0x0C,
		KC_RETURN = 0x0D,
		//undefined    = 0x0E, 
		//undefined    = 0x0F,
		KC_SHIFT = 0x10,
		KC_CONTROL = 0x11,
		KC_MENU = 0x12,	// alt
		KC_PAUSE = 0x13,
		KC_CAPITAL = 0x14,	// caps lock
		KC_KANA = 0x15,
		KC_HANGUEL = 0x15,
		KC_HANGUL = 0x15,
		//undefined    = 0x16,
		KC_JUNJA = 0x17,
		KC_FINAL = 0x18,
		KC_HANJA = 0x19,
		KC_KANJI = 0x19,
		//undefined    = 0x1A,
		KC_ESCAPE = 0x1B,
		KC_CONVERT = 0x1C,
		KC_NOCONVERT = 0x1D,
		KC_ACCEPT = 0x1E,
		KC_MODECHANGE = 0x1F,
		KC_SPACE = 0x20,
		KC_PRIOR = 0x21, // pageup
		KC_NEXT = 0x22, // pagedown
		KC_END = 0x23,
		KC_HOME = 0x24,
		KC_LEFT = 0x25,
		KC_UP = 0x26,
		KC_RIGHT = 0x27,
		KC_DOWN = 0x28,
		KC_SELECT = 0x29,
		KC_PRINT = 0x2A,
		KC_EXECUTE = 0x2B,
		KC_SNAPSHOT = 0x2C,	// printscreen
		KC_INSERT = 0x2D,
		KC_DELETE = 0x2E,
		KC_HELP = 0x2F,
		KC_0 = 0x30,
		KC_1 = 0x31,
		KC_2 = 0x32,
		KC_3 = 0x33,
		KC_4 = 0x34,
		KC_5 = 0x35,
		KC_6 = 0x36,
		KC_7 = 0x37,
		KC_8 = 0x38,
		KC_9 = 0x39,
		//undefined    = 0x3A,
		//undefined    = 0x3B,
		//undefined    = 0x3C,
		//undefined    = 0x3D,
		//undefined    = 0x3E,
		//undefined    = 0x3F,
		//undefined    = 0x40,
		KC_A = 0x41,
		KC_B = 0x42,
		KC_C = 0x43,
		KC_D = 0x44,
		KC_E = 0x45,
		KC_F = 0x46,
		KC_G = 0x47,
		KC_H = 0x48,
		KC_I = 0x49,
		KC_J = 0x4A,
		KC_K = 0x4B,
		KC_L = 0x4C,
		KC_M = 0x4D,
		KC_N = 0x4E,
		KC_O = 0x4F,
		KC_P = 0x50,
		KC_Q = 0x51,
		KC_R = 0x52,
		KC_S = 0x53,
		KC_T = 0x54,
		KC_U = 0x55,
		KC_V = 0x56,
		KC_W = 0x57,
		KC_X = 0x58,
		KC_Y = 0x59,
		KC_Z = 0x5A,
		KC_LWIN = 0x5B,
		KC_RWIN = 0x5C,
		KC_APPS = 0x5D,    // applications key
		//reserved     = 0x5E,    // applications key
		KC_SLEEP = 0x5F,
		KC_NUMPAD0 = 0x60,
		KC_NUMPAD1 = 0x61,
		KC_NUMPAD2 = 0x62,
		KC_NUMPAD3 = 0x63,
		KC_NUMPAD4 = 0x64,
		KC_NUMPAD5 = 0x65,
		KC_NUMPAD6 = 0x66,
		KC_NUMPAD7 = 0x67,
		KC_NUMPAD8 = 0x68,
		KC_NUMPAD9 = 0x69,
		KC_MULTIPLY = 0x6A,    // *
		KC_ADD = 0x6B,    // +
		KC_SEPARATOR = 0x6C,    // 
		KC_SUBTRACT = 0x6D,    // -
		KC_DECIMAL = 0x6E,    // .
		KC_DIVIDE = 0x6F,    // /
		KC_F1 = 0x70,
		KC_F2 = 0x71,
		KC_F3 = 0x72,
		KC_F4 = 0x73,
		KC_F5 = 0x74,
		KC_F6 = 0x75,
		KC_F7 = 0x76,
		KC_F8 = 0x77,
		KC_F9 = 0x78,
		KC_F10 = 0x79,
		KC_F11 = 0x7A,
		KC_F12 = 0x7B,
		KC_F13 = 0x7C,
		KC_F14 = 0x7D,
		KC_F15 = 0x7E,
		KC_F16 = 0x7F,
		KC_F17 = 0x80,
		KC_F18 = 0x81,
		KC_F19 = 0x82,
		KC_F20 = 0x83,
		KC_F21 = 0x84,
		KC_F22 = 0x85,
		KC_F23 = 0x86,
		KC_F24 = 0x87,
		//unassigned   = 0x88,
		//unassigned   = 0x89,
		//unassigned   = 0x8A,
		//unassigned   = 0x8B,
		//unassigned   = 0x8C,
		//unassigned   = 0x8D,
		//unassigned   = 0x8E,
		//unassigned   = 0x8F,
		KC_NUMLOCK = 0x90,
		KC_SCROLL = 0x91,    // scroll lock
		//OEM specific = 0x92,    
		//OEM specific = 0x93,    
		//OEM specific = 0x94,    
		//OEM specific = 0x95,    
		//OEM specific = 0x96,    
		//unassigned   = 0x97,
		//unassigned   = 0x98,
		//unassigned   = 0x99,
		//unassigned   = 0x9A,
		//unassigned   = 0x9B,
		//unassigned   = 0x9C, 
		//unassigned   = 0x9D,
		//unassigned   = 0x9E,
		//unassigned   = 0x9F,
		KC_LSHIFT = 0xA0,
		KC_RSHIFT = 0xA1,
		KC_LCONTROL = 0xA2,
		KC_RCONTROL = 0xA3,
		KC_LMENU = 0xA4,
		KC_RMENU = 0xA5,
		KC_BROWSER_BACK = 0xA6,
		KC_BROWSER_FORWARD = 0xA7,
		KC_BROWSER_REFRESH = 0xA8,
		KC_BROWSER_STOP = 0xA9,
		KC_BROWSER_SEARCH = 0xAA,
		KC_BROWSER_FAVORITES = 0xAB,
		KC_BROWSER_HOME = 0xAC,
		KC_VOLUME_MUTE = 0xAD,
		KC_VOLUME_DOWN = 0xAE,
		KC_VOLUME_UP = 0xAF,
		KC_MEDIA_NEXT_TRACK = 0xB0,
		KC_MEDIA_PREV_TRACK = 0xB1,
		KC_MEDIA_STOP = 0xB2,
		KC_MEDIA_PLAY_PAUSE = 0xB3,
		KC_LAUNCH_MAIL = 0xB4,
		KC_LAUNCH_MEDIA_SELECT = 0xB5,
		KC_LAUNCH_APP1 = 0xB6,
		KC_LAUNCH_APP2 = 0xB7,
		//reserved       = 0xB8,    
		//reserved       = 0xB9,    
		KC_OEM_1 = 0xBA,
		KC_OEM_PLUS = 0xBB,
		KC_OEM_COMMA = 0xBC,
		KC_OEM_MINUS = 0xBD,
		KC_OEM_PERIOD = 0xBE,
		KC_OEM_2 = 0xBF,
		KC_OEM_3 = 0xC0,
		// reserved	   = 0xC1 - 0xD7
		// unassigned  = 0xD8 - 0xDA
		KC_OEM_4 = 0xDB,
		KC_OEM_5 = 0xDC,
		KC_OEM_6 = 0xDD,
		KC_OEM_7 = 0xDE,
		KC_OEM_8 = 0xDF,
		// reserved	   = 0xE0
		// OEM specific= 0xE1
		KC_OEM_102 = 0xE2,
		// OEM specific= 0xE3,    
		// OEM specific= 0xE4,    
		KC_PROCESSKEY = 0xE5,
		// OEM specific= 0xE6,    
		KC_PACKET = 0xE7,
		// unassigned  = 0xE8,    
		// OEM specific= 0xE9 - 0xF5,
		KC_ATTN = 0xF6,
		KC_CRSEL = 0xF7,
		KC_EXSEL = 0xF8,
		KC_EREOF = 0xF9,
		KC_PLAY = 0xFA,
		KC_ZOOM = 0xFB,
		KC_NONAME = 0xFC,
		KC_PA1 = 0xFD,
		KC_OEM_CLEAR = 0xFE,
		KC_MAX_CODE = 0xFF
	};
	//////////////////////////////////////////////////////////////////////////
	enum InputEventType
	{
		IET_KEY,
		IET_MOUSE_BUTTON,
		IET_MOUSE_WHELL,
		IET_MOUSE_MOVE,
		IET_MOUSE_POSITION,
		IET_MOUSE_ENTER,
		IET_MOUSE_LEAVE
	};
	//////////////////////////////////////////////////////////////////////////
	struct InputKeyEvent
	{
		InputEventType type;

		float x;
		float y;
		KeyCode key;
		WChar code;
		bool isDown;
		bool isRepeat;
	};
	//////////////////////////////////////////////////////////////////////////
	struct InputMouseButtonEvent
	{
		InputEventType type;

		unsigned int touchId;
		float x;
		float y;
		unsigned int button;
		float pressure;
		bool isDown;
		bool isPressed;
	};
	//////////////////////////////////////////////////////////////////////////
	struct InputMouseWheelEvent
	{
		InputEventType type;

		float x;
		float y;
		unsigned int button;
		int wheel;
	};
	//////////////////////////////////////////////////////////////////////////
	struct InputMouseMoveEvent
	{
		InputEventType type;

		unsigned int touchId;
		float x;
		float y;
		float dx;
		float dy;
		float pressure;
	};
	//////////////////////////////////////////////////////////////////////////
	struct InputMousePositionEvent
	{
		InputEventType type;

		unsigned int touchId;
		float x;
		float y;
		float pressure;
	};
	//////////////////////////////////////////////////////////////////////////
	union InputUnionEvent
	{
		InputEventType type;

		InputKeyEvent key;
		InputMouseButtonEvent button;
		InputMouseWheelEvent wheel;
		InputMouseMoveEvent move;
		InputMousePositionEvent position;
	};
	//////////////////////////////////////////////////////////////////////////
	class InputSystemHandler
	{
	public:
		InputSystemHandler(){};
		virtual ~InputSystemHandler(){};

	public:
		virtual bool handleKeyEvent( const InputKeyEvent & _event ) = 0;

	public:
		virtual bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) = 0;
		virtual bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) = 0;
		virtual bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) = 0;
		virtual bool handleMouseMove( const InputMouseMoveEvent & _event ) = 0;
		virtual bool handleMouseWheel( const InputMouseWheelEvent & _event ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class InputMousePositionProvider
	{
	public:
		InputMousePositionProvider(){};
		virtual ~InputMousePositionProvider(){};

	public:
		virtual void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class InputServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("InputService")

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
		virtual void update() = 0;

	public:	
		virtual bool isAltDown() const = 0;
		virtual bool isShiftDown() const = 0;
		virtual bool isCtrlDown() const = 0;

	public:
		virtual bool isKeyDown( uint32_t _keyCode ) const = 0;
		virtual bool isExclusiveKeyDown( uint32_t _keyCode ) const = 0;
		virtual bool isAnyKeyDown() const = 0;

	public:
		virtual bool isAnyMouseButtonDown() const = 0;
		virtual bool isMouseButtonDown( uint32_t _buttonId ) const = 0;

		virtual void setCursorPosition( uint32_t _touchId, const mt::vec2f & _point ) = 0;
		virtual const mt::vec2f & getCursorPosition( uint32_t _touchId ) const = 0;
		virtual bool validCursorPosition( float _x, float _y ) const = 0;

		virtual void addMousePositionProvider( InputMousePositionProvider * _provider ) = 0;
		virtual void removeMousePositionProvider( InputMousePositionProvider * _provider ) = 0;

	public:
		virtual void onFocus( bool _focus ) = 0;

	public:
		virtual void pushEvent( const InputUnionEvent & _event ) = 0;

	public:
		inline void pushMouseMoveEvent( unsigned int _touchId, float _x, float _y, float _dx, float _dy, float _pressure )
		{
			InputUnionEvent event;
			event.move.type = IET_MOUSE_MOVE;

			event.move.touchId = _touchId;
			event.move.x = _x;
			event.move.y = _y;
			event.move.dx = _dx;
			event.move.dy = _dy;
			event.move.pressure = _pressure;

			this->pushEvent( event );
		}

		inline void pushMouseButtonEvent( unsigned int _touchId, float _x, float _y, unsigned int _button, float _pressure, bool _isDown, bool _isPressed )
		{
			InputUnionEvent event;
			event.button.type = IET_MOUSE_BUTTON;

			event.button.touchId = _touchId;
			event.button.x = _x;
			event.button.y = _y;
			event.button.button = _button;
			event.button.pressure = _pressure;
			event.button.isDown = _isDown;
			event.button.isPressed = _isPressed;

			this->pushEvent( event );
		}

		inline void pushKeyEvent( float _x, float _y, KeyCode _key, WChar _code, bool _isDown, bool _repeating )
		{
			InputUnionEvent event;
			event.key.type = IET_KEY;

			event.key.x = _x;
			event.key.y = _y;
			event.key.key = _key;
			event.key.code = _code;
			event.key.isDown = _isDown;
			event.key.isRepeat = _repeating;

			this->pushEvent( event );
		}
				
		inline void pushMouseLeaveEvent( unsigned int _touchId, float _x, float _y, float _pressure )
		{
			InputUnionEvent event;
			event.position.type = IET_MOUSE_LEAVE;

			event.position.touchId = _touchId;
			event.position.x = _x;
			event.position.y = _y;
			event.position.pressure = _pressure;

			this->pushEvent( event );
		}

		//////////////////////////////////////////////////////////////////////////
		inline void pushMouseEnterEvent( unsigned int _touchId, float _x, float _y, float _pressure )
		{
			InputUnionEvent event;
			event.position.type = IET_MOUSE_ENTER;

			event.position.touchId = _touchId;
			event.position.x = _x;
			event.position.y = _y;
			event.position.pressure = _pressure;

			this->pushEvent( event );
		}
		//////////////////////////////////////////////////////////////////////////
		inline void pushMouseWheelEvent( float _x, float _y, unsigned int _button, int _whell )
		{
			InputUnionEvent event;
			event.wheel.type = IET_MOUSE_WHELL;

			event.wheel.x = _x;
			event.wheel.y = _y;
			event.wheel.button = _button;
			event.wheel.wheel = _whell;

			this->pushEvent( event );
		}
		//////////////////////////////////////////////////////////////////////////
		inline void pushMousePositionEvent( unsigned int _touchId, float _x, float _y, float _pressure )
		{
			InputUnionEvent event;
			event.position.type = IET_MOUSE_POSITION;

			event.position.touchId = _touchId;
			event.position.x = _x;
			event.position.y = _y;
			event.position.pressure = _pressure;

			this->pushEvent( event );
		}
	};

#   define INPUT_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::InputServiceInterface)
}
