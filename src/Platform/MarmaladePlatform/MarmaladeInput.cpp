#   include "MarmaladeInput.h"

#	include "Logger/Logger.h"

#	include "Config/String.h"

#	include <s3eSurface.h>
#	include <s3eDevice.h>


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeInput::MarmaladeInput()
        : m_serviceProvider(nullptr)
		, m_keysIterator(0)
		, m_lastKey( s3eKeyFirst )
		, m_width(1024.f)
		, m_height(768.f)
		, m_showKeyboard(false)
		, m_alwaysKeyboard(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MarmaladeInput::~MarmaladeInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeInput::getServiceProvider()
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::initialize()
    {
        int32 pointer = s3ePointerGetInt( S3E_POINTER_AVAILABLE );

		if( pointer == S3E_FALSE )
        {
            return false;
        }
		
        int32 multiTouch = s3ePointerGetInt( S3E_POINTER_MULTI_TOUCH_AVAILABLE );

		if( multiTouch == S3E_TRUE )
        {
			if( s3ePointerRegister( S3E_POINTER_TOUCH_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchEvent, this ) == S3E_RESULT_ERROR )
			{
				s3ePointerError p_err = s3ePointerGetError();
				const char * p_str = s3ePointerGetErrorString();

				LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_POINTER_TOUCH_EVENT err %d:%s"
					, p_err
					, p_str
					);
			}

			if( s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchMotionEvent, this ) == S3E_RESULT_ERROR )
			{
				s3ePointerError p_err = s3ePointerGetError();
				const char * p_str = s3ePointerGetErrorString();

				LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_POINTER_TOUCH_MOTION_EVENT err %d:%s"
					, p_err
					, p_str
					);
			}
        }
		else
		{
			if( s3ePointerRegister( S3E_POINTER_BUTTON_EVENT, (s3eCallback)&MarmaladeInput::s_pointerButtonEvent, this ) == S3E_RESULT_ERROR )
			{
				s3ePointerError p_err = s3ePointerGetError();
				const char * p_str = s3ePointerGetErrorString();

				LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_POINTER_BUTTON_EVENT err %d:%s"
					, p_err
					, p_str
					);
			}

			if( s3ePointerRegister( S3E_POINTER_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerMotionEvent, this ) == S3E_RESULT_ERROR )
			{
				s3ePointerError p_err = s3ePointerGetError();
				const char * p_str = s3ePointerGetErrorString();

				LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_POINTER_MOTION_EVENT err %d:%s"
					, p_err
					, p_str
					);
			}
		}
        
		if( s3eKeyboardRegister( S3E_KEYBOARD_KEY_EVENT, &MarmaladeInput::s_keyboardKeyEvent, this ) == S3E_RESULT_ERROR )
		{
			s3eKeyboardError p_err = s3eKeyboardGetError();
			const char * p_str = s3eKeyboardGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_KEYBOARD_KEY_EVENT err %d:%s"
				, p_err
				, p_str
				);
		}
		
		if( s3eKeyboardRegister( S3E_KEYBOARD_CHAR_EVENT, &MarmaladeInput::s_keyboardCharEvent, this ) == S3E_RESULT_ERROR )
		{
			s3eKeyboardError p_err = s3eKeyboardGetError();
			const char * p_str = s3eKeyboardGetErrorString();

			LOGGER_ERROR( m_serviceProvider )("MarmaladeInput::initialize invalid regist S3E_KEYBOARD_CHAR_EVENT err %d:%s"
				, p_err
				, p_str
				);
		}
        
        this->fillKeys_();

        for( uint32_t i = 0; i != MENGINE_MAX_TOUCHES; ++i )
        {
            TouchDesc & touch = m_touches[i];

            touch.point.x = 0.f;
            touch.point.y = 0.f;
            touch.active = false;
        }

		this->updateSurfaceResolution();

		int32 deviceId = s3eDeviceGetInt( S3E_DEVICE_OS );

		if( deviceId == S3E_OS_ID_WINDOWS || deviceId == S3E_OS_ID_OSX || deviceId == S3E_OS_ID_LINUX  )
		{
			m_alwaysKeyboard = true;
		}
		else
		{
			m_alwaysKeyboard = false;
		}

		if( m_alwaysKeyboard == true )
		{
			s3eKeyboardSetInt( S3E_KEYBOARD_GET_CHAR, 1 );
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeInput::finalize()
	{
		int32 pointer = s3ePointerGetInt( S3E_POINTER_AVAILABLE );

		if( pointer == S3E_FALSE )
		{
			return;
		}

		int32 multiTouch = s3ePointerGetInt( S3E_POINTER_MULTI_TOUCH_AVAILABLE );

		if( multiTouch == S3E_TRUE )
		{
			s3ePointerUnRegister( S3E_POINTER_TOUCH_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchEvent );
			s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchMotionEvent );
		}
		else
		{
			s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)&MarmaladeInput::s_pointerButtonEvent );
			s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerMotionEvent );
		}

		s3eKeyboardUnRegister( S3E_KEYBOARD_KEY_EVENT, (s3eCallback)&MarmaladeInput::s_keyboardKeyEvent );
		s3eKeyboardUnRegister( S3E_KEYBOARD_CHAR_EVENT, (s3eCallback)&MarmaladeInput::s_keyboardCharEvent );
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::update()
    {
        if( m_alwaysKeyboard == false && m_showKeyboard == true )
        {
            s3eKeyboardSetInt(S3E_KEYBOARD_GET_CHAR, 1);

            m_showKeyboard = false;
        }

        s3eResult keyboardUpdate = s3eKeyboardUpdate();

        if( keyboardUpdate != S3E_RESULT_SUCCESS )
        {
            return false;
        }

        s3eResult pointerUpdate = s3ePointerUpdate();

        if( pointerUpdate != S3E_RESULT_SUCCESS )
        {
            return false;
        }

		s3eDeviceYield( 0 );

		mt::vec2f point;
		this->getCorrectPoint( 0, point );

		for( uint32_t i = 0; i != m_keysIterator; ++i )
		{
			const KeyEventDesc & desc = m_keyEvents[i];

			s3eKey key = desc.key;

			if( key == s3eKeyFirst )
			{
				TMapWCharCode::const_iterator it_found = m_wcharCodes.find( desc.ch );

				KeyCode code = KC_UNASSIGNED;

				if( it_found != m_wcharCodes.end() )
				{
					code = it_found->second;
				}

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, desc.ch, true, false );

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, desc.ch, false, false );
			}
			else
			{
				KeyCode code = this->getKeyCode_( key );
				bool isDown = desc.pressed != 0;

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, desc.ch, isDown, false );
			}
		}

		m_keysIterator = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::updateSurfaceResolution()
    {
        m_width = (float)s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
        m_height = (float)s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );
    }
    //////////////////////////////////////////////////////////////////////////    
    void MarmaladeInput::showKeyboard( bool _value )
    {
        m_showKeyboard = _value;

        if( m_alwaysKeyboard == false && m_showKeyboard == false )
        {
            s3eKeyboardSetInt( S3E_KEYBOARD_GET_CHAR, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::isKeyDown( Menge::KeyCode _code ) const
    {
        s3eKey key = this->gets3eKey_( _code );

        int32 state = s3eKeyboardGetState( key );

        bool isDown = (state & S3E_KEY_STATE_DOWN) != 0;

        return isDown;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::isAnyKeyDown() const 
    {
        s3eKey key = s3eKeyboardAnyKey();

        bool isDown = key != 0;

        return isDown;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeInput::getCorrectPoint( uint32 _touch, mt::vec2f & _point ) const
	{ 
		int32 cursorX = s3ePointerGetTouchX( _touch );
		int32 cursorY = s3ePointerGetTouchY( _touch );

		mt::vec2f point;
		this->correctPoint_( cursorX, cursorY, point );

		_point = point;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeInput::correctPoint_( int32 _x, int32 _y, mt::vec2f & _point ) const
	{
		float x = (float)_x / m_width;
		float y = (float)_y / m_height;

		_point.x = x;
		_point.y = y;	
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_keyboardKeyEvent( void * _systemData, void * _userData )
	{
		s3eKeyboardEvent * event = (s3eKeyboardEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->keyboardKeyEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_keyboardCharEvent( void * _systemData, void * _userData )
	{
		s3eKeyboardCharEvent * event = (s3eKeyboardCharEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->keyboardCharEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_pointerTouchEvent( void * _systemData, void * _userData )
	{
		s3ePointerTouchEvent * event = (s3ePointerTouchEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->pointerTouchEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_pointerTouchMotionEvent( void * _systemData, void * _userData )
	{
		s3ePointerTouchMotionEvent * event = (s3ePointerTouchMotionEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->pointerTouchMotionEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_pointerButtonEvent( void * _systemData, void * _userData )
	{
		s3ePointerEvent * event = (s3ePointerEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->pointerButtonEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::s_pointerMotionEvent( void * _systemData, void * _userData )
	{
		s3ePointerMotionEvent * event = (s3ePointerMotionEvent *)_systemData;
		MarmaladeInput * input = (MarmaladeInput *)_userData;

		int32 result = input->pointerMotionEvent( event );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::keyboardCharEvent( s3eKeyboardCharEvent * _event )
	{
		if( m_keysIterator == MENGINE_MAX_KEYS )
		{
			return 0;
		}

		if( m_keysIterator == 0 )
		{
			m_keyEvents[m_keysIterator].key = m_lastKey;
			m_keyEvents[m_keysIterator].pressed = 1;
			m_keyEvents[m_keysIterator].ch = _event->m_Char;
			
			++m_keysIterator;

			return 0;
		}

		if( m_keyEvents[m_keysIterator - 1].ch == 0 )
		{
			m_keyEvents[m_keysIterator - 1].ch = _event->m_Char;
		}
		else
		{
			m_keyEvents[m_keysIterator].key = m_keyEvents[m_keysIterator - 1].key;
			m_keyEvents[m_keysIterator].pressed = 1;
			m_keyEvents[m_keysIterator].ch = _event->m_Char;

			++m_keysIterator;
		}				

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	int32 MarmaladeInput::keyboardKeyEvent( s3eKeyboardEvent * _event )
    {
		if( m_keysIterator == MENGINE_MAX_KEYS )
		{
			return 0;
		}

		if( _event->m_Pressed == 1 )
		{
			m_lastKey = _event->m_Key;
		}

		s3eWChar ch = 0;
		if( m_keysIterator > 0 && _event->m_Pressed == 0 )
		{
			ch = m_keyEvents[m_keysIterator - 1].ch;
		}

		m_keyEvents[m_keysIterator].key = _event->m_Key;
		m_keyEvents[m_keysIterator].pressed = _event->m_Pressed;
		m_keyEvents[m_keysIterator].ch = ch;

		++m_keysIterator;

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32 MarmaladeInput::pointerTouchEvent( s3ePointerTouchEvent * _event )
    {		
        uint32 touchId = _event->m_TouchID;

        bool isDown = _event->m_Pressed != 0;

        mt::vec2f point;
		this->correctPoint_( _event->m_x, _event->m_y, point );

		if( this->setTouch_( touchId, point, isDown ) == false )
        {
            return 0;
        }

		if( SERVICE_EXIST( m_serviceProvider, Menge::InputServiceInterface ) == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseButtonEvent( touchId, point.x, point.y, 0, 0.f, isDown );
		}

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32 MarmaladeInput::pointerTouchMotionEvent( s3ePointerTouchMotionEvent * _event )
    {
        uint32 touchId = _event->m_TouchID;

        TouchDesc desc;
        if( this->getTouch_( touchId, desc ) == false )
        {
            return 0;
        }

        mt::vec2f point;
		this->correctPoint_( _event->m_x, _event->m_y, point );

        mt::vec2f diff = point - desc.point;

		this->setTouch_( touchId, point, desc.active );

		if( SERVICE_EXIST( m_serviceProvider, Menge::InputServiceInterface ) == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseMoveEvent( touchId, point.x, point.y, diff.x, diff.y, 0.f );
		}

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32 MarmaladeInput::pointerButtonEvent( s3ePointerEvent * _event )
    {
		bool isDown = _event->m_Pressed != 0;

		mt::vec2f point;
		this->correctPoint_( _event->m_x, _event->m_y, point );

		this->setTouch_( 0, point, isDown );

		if( SERVICE_EXIST( m_serviceProvider, Menge::InputServiceInterface ) == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseButtonEvent( 0, point.x, point.y, 0, 0.f, isDown );
		}

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int32 MarmaladeInput::pointerMotionEvent( s3ePointerMotionEvent * _event )
    {
        TouchDesc desc;
		if( this->getTouch_( 0, desc ) == false )
        {
            return 0;
        }

		mt::vec2f point;
		this->correctPoint_( _event->m_x, _event->m_y, point );

        mt::vec2f diff = point - desc.point;

		this->setTouch_( 0, point, desc.active );

		if( SERVICE_EXIST( m_serviceProvider, Menge::InputServiceInterface ) == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseMoveEvent( 0, point.x, point.y, diff.x, diff.y, 0.f );
		}

		return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::setTouch_( uint32 _touchId, const mt::vec2f & _point, bool _active )
    {
        if( _touchId >= MENGINE_MAX_TOUCHES )
        {
            return false;
        }

        TouchDesc & desc = m_touches[_touchId];
        desc.point = _point;
        desc.active = _active;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::getTouch_( uint32 _touchId, TouchDesc & _desc )
    {
        if( _touchId >= MENGINE_MAX_TOUCHES )
        {
            return false;
        }

        TouchDesc & desc = m_touches[_touchId];

        _desc = desc;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::fillKeys_()
    {
        for( uint32_t i = 0; i != 255; ++i )
        {
            m_keys[i] = KC_UNASSIGNED;
        }

		m_keys[s3eKeyEnter] = Menge::KC_RETURN;
        m_keys[s3eKeyBack] = Menge::KC_ESCAPE;
        m_keys[s3eKeyMenu] = Menge::KC_MENU;

        m_keys[s3eKeyTab] = Menge::KC_TAB;
        m_keys[s3eKeyLeftShift] = Menge::KC_LSHIFT;
		m_keys[s3eKeyRightShift] = Menge::KC_RSHIFT;
        m_keys[s3eKeyLeftControl] = Menge::KC_LCONTROL;
		m_keys[s3eKeyRightControl] = Menge::KC_RCONTROL;
		m_keys[s3eKeyLeftAlt] = Menge::KC_LMENU;
		m_keys[s3eKeyRightAlt] = Menge::KC_RMENU;
        m_keys[s3eKeyPause] = Menge::KC_PAUSE;
        m_keys[s3eKeyCapsLock] = Menge::KC_CAPITAL;
        m_keys[s3eKeyEsc] = Menge::KC_ESCAPE;
        m_keys[s3eKeyPageUp] = Menge::KC_PRIOR;
        m_keys[s3eKeyPageDown] = Menge::KC_NEXT;
        m_keys[s3eKeyEnd] = Menge::KC_END;
        m_keys[s3eKeyHome] = Menge::KC_HOME;
        m_keys[s3eKeyLeft] = Menge::KC_LEFT;
        m_keys[s3eKeyUp] = Menge::KC_UP;
        m_keys[s3eKeyRight] = Menge::KC_RIGHT;
        m_keys[s3eKeyDown] = Menge::KC_DOWN;
        m_keys[s3eKeyInsert] = Menge::KC_INSERT;
        m_keys[s3eKeyDelete] = Menge::KC_DELETE;

        m_keys[s3eKey0] = Menge::KC_0;
        m_keys[s3eKey1] = Menge::KC_1;
        m_keys[s3eKey2] = Menge::KC_2;
        m_keys[s3eKey3] = Menge::KC_3;
        m_keys[s3eKey4] = Menge::KC_4;
        m_keys[s3eKey5] = Menge::KC_5;
        m_keys[s3eKey6] = Menge::KC_6;
        m_keys[s3eKey7] = Menge::KC_7;
        m_keys[s3eKey8] = Menge::KC_8;
        m_keys[s3eKey9] = Menge::KC_9;

        m_keys[s3eKeyA] = Menge::KC_A;
        m_keys[s3eKeyB] = Menge::KC_B;
        m_keys[s3eKeyC] = Menge::KC_C;
        m_keys[s3eKeyD] = Menge::KC_D;
        m_keys[s3eKeyE] = Menge::KC_E;
        m_keys[s3eKeyF] = Menge::KC_F;
        m_keys[s3eKeyG] = Menge::KC_G;
        m_keys[s3eKeyH] = Menge::KC_H;
        m_keys[s3eKeyI] = Menge::KC_I;
        m_keys[s3eKeyJ] = Menge::KC_J;
        m_keys[s3eKeyK] = Menge::KC_K;
        m_keys[s3eKeyL] = Menge::KC_L;
        m_keys[s3eKeyM] = Menge::KC_M;
        m_keys[s3eKeyN] = Menge::KC_N;
        m_keys[s3eKeyO] = Menge::KC_O;
        m_keys[s3eKeyP] = Menge::KC_P;
        m_keys[s3eKeyQ] = Menge::KC_Q;
        m_keys[s3eKeyR] = Menge::KC_R;
        m_keys[s3eKeyS] = Menge::KC_S;
        m_keys[s3eKeyT] = Menge::KC_T;
        m_keys[s3eKeyU] = Menge::KC_U;
        m_keys[s3eKeyV] = Menge::KC_V;
        m_keys[s3eKeyW] = Menge::KC_W;
        m_keys[s3eKeyX] = Menge::KC_X;
        m_keys[s3eKeyY] = Menge::KC_Y;
        m_keys[s3eKeyZ] = Menge::KC_Z;

        m_keys[s3eKeyNumPad0] = Menge::KC_NUMPAD0;
        m_keys[s3eKeyNumPad1] = Menge::KC_NUMPAD1;
        m_keys[s3eKeyNumPad2] = Menge::KC_NUMPAD2;
        m_keys[s3eKeyNumPad3] = Menge::KC_NUMPAD3;
        m_keys[s3eKeyNumPad4] = Menge::KC_NUMPAD4;
        m_keys[s3eKeyNumPad5] = Menge::KC_NUMPAD5;
        m_keys[s3eKeyNumPad6] = Menge::KC_NUMPAD6;
        m_keys[s3eKeyNumPad7] = Menge::KC_NUMPAD7;
        m_keys[s3eKeyNumPad8] = Menge::KC_NUMPAD8;
        m_keys[s3eKeyNumPad9] = Menge::KC_NUMPAD9;

        m_keys[s3eKeyNumPadMinus] = Menge::KC_SUBTRACT;
        m_keys[s3eKeyNumPadPlus] = Menge::KC_ADD;

        m_keys[s3eKeySlash] = Menge::KC_DIVIDE;

        m_keys[s3eKeyF1] = Menge::KC_F1;
        m_keys[s3eKeyF2] = Menge::KC_F2;
        m_keys[s3eKeyF3] = Menge::KC_F3;
        m_keys[s3eKeyF4] = Menge::KC_F4;
        m_keys[s3eKeyF5] = Menge::KC_F5;
        m_keys[s3eKeyF6] = Menge::KC_F6;
        m_keys[s3eKeyF7] = Menge::KC_F7;
        m_keys[s3eKeyF8] = Menge::KC_F8;
        m_keys[s3eKeyF9] = Menge::KC_F9;
        m_keys[s3eKeyF10] = Menge::KC_F10;
        m_keys[s3eKeyF11] = Menge::KC_F11;
        m_keys[s3eKeyF12] = Menge::KC_F12;

		m_keys[s3eKeyLeftBracket] = Menge::KC_OEM_4;
		m_keys[s3eKeyRightBracket] = Menge::KC_OEM_6;
		m_keys[s3eKeyEquals] = Menge::KC_OEM_PLUS;
		m_keys[s3eKeyMinus] = Menge::KC_OEM_MINUS;

        m_keys[s3eKeyNumLock] = Menge::KC_NUMLOCK;

        for( uint32_t i = 0; i != 255; ++i )
        {
            KeyCode code = m_keys[i];

            if( code == 255 )
            {
                continue;
            }

            m_codes[code] = (s3eKey)i;
        }

		s3eWChar wc_return = (s3eWChar)10;
		m_wcharCodes[wc_return] = Menge::KC_RETURN;
    }
    //////////////////////////////////////////////////////////////////////////
    KeyCode MarmaladeInput::getKeyCode_( s3eKey _key ) const
    {
        if( _key >= 0xFF )
        {
            return KC_UNASSIGNED;
        }

        KeyCode code = m_keys[_key];
        
        return code; 
    }
    //////////////////////////////////////////////////////////////////////////
    s3eKey MarmaladeInput::gets3eKey_( KeyCode _code ) const
    {
        if( _code >= 0xFF )
        {
            return s3eKeyFirst;
        }

        s3eKey key = m_codes[_code];

        if( key >= 0xFF )
        {
            return s3eKeyFirst;
        }

        return key; 
    }
}