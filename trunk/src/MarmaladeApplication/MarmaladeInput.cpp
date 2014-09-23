#   include "MarmaladeInput.h"

#	include <s3eSurface.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeInput::MarmaladeInput()
        : m_serviceProvider(nullptr)
        , m_showKeyboard(false)
		, m_width(1024.f)
		, m_height(768.f)
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

        if( pointer == 0 )
        {
            return false;
        }

        int32 multiTouch = s3ePointerGetInt( S3E_POINTER_MULTI_TOUCH_AVAILABLE );

        if( multiTouch == 1 )
        {
            s3ePointerRegister( S3E_POINTER_TOUCH_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchEvent, this );
            s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerTouchMotionEvent, this );
        }
		else
		{
			s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)&MarmaladeInput::s_pointerButtonEvent, this );
			s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)&MarmaladeInput::s_pointerMotionEvent, this );
		}
        
        s3eKeyboardRegister( S3E_KEYBOARD_KEY_EVENT, (s3eCallback)&MarmaladeInput::s_keyboardKeyEvent, this );
        s3eKeyboardRegister( S3E_KEYBOARD_CHAR_EVENT, (s3eCallback)&MarmaladeInput::s_keyboardCharEvent, this );
        
        this->fillKeys_();

        for( uint32_t i = 0; i != MENGINE_MAX_TOUCHES; ++i )
        {
            TouchDesc & touch = m_touches[i];

            touch.point.x = 0.f;
            touch.point.y = 0.f;
            touch.active = false;
        }

		m_width = (float)s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
		m_height = (float)s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeInput::update()
    {
        if( m_showKeyboard == true )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////    
    void MarmaladeInput::showKeyboard( bool _value )
    {
        m_showKeyboard = _value;

        if( m_showKeyboard == false )
        {
            s3eKeyboardSetInt(S3E_KEYBOARD_GET_CHAR, 0);
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
	void MarmaladeInput::correctPoint_( int32 _x, int32 _y, mt::vec2f & _point ) const
	{
		float x = (float)_x / m_width;
		float y = (float)_y / m_height;

		_point.x = x;
		_point.y = y;
	}
	//////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_keyboardKeyEvent( s3eKeyboardEvent * _event, MarmaladeInput * _input )
    {
        int32 cursorX = s3ePointerGetTouchX( 0 );
        int32 cursorY = s3ePointerGetTouchY( 0 );
        
		mt::vec2f point;
		_input->correctPoint_( cursorX, cursorY, point );

        s3eWChar ch = 0;

        KeyCode code = _input->getKeyCode_( _event->m_Key );
        bool isDown = _event->m_Pressed != 0;
                
        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onKeyEvent( point, code, ch, isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_keyboardCharEvent( s3eKeyboardCharEvent * _event, MarmaladeInput * _input )
    {
        int32 cursorX = s3ePointerGetTouchX( 0 );
        int32 cursorY = s3ePointerGetTouchY( 0 );

		mt::vec2f point;
		_input->correctPoint_( cursorX, cursorY, point );

        s3eWChar ch = _event->m_Char;
        s3eKey code = s3eKeyFirst;

        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onKeyEvent( point, code, ch, true );

        INPUT_SERVICE(serviceProvider)
            ->onKeyEvent( point, code, ch, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_pointerTouchEvent( s3ePointerTouchEvent * _event, MarmaladeInput * _input )
    {		
        uint32 touchId = _event->m_TouchID;

        bool isDown = _event->m_Pressed != 0;

        mt::vec2f point;
		_input->correctPoint_( _event->m_x, _event->m_y, point );

        if( _input->setTouch_( touchId, point, isDown ) == false )
        {
            return;
        }
        
        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onMouseButtonEvent( touchId, point, 0, isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_pointerTouchMotionEvent( s3ePointerTouchMotionEvent * _event, MarmaladeInput * _input )
    {
        uint32 touchId = _event->m_TouchID;

        TouchDesc desc;
        if( _input->getTouch_( touchId, desc ) == false )
        {
            return;
        }

        mt::vec2f point;
		_input->correctPoint_( _event->m_x, _event->m_y, point );

        mt::vec2f diff = point - desc.point;

        _input->setTouch_( touchId, point, desc.active );

        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onMouseMove( touchId, point, diff.x, diff.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_pointerButtonEvent( s3ePointerEvent * _event, MarmaladeInput * _input )
    {
        bool isDown = _event->m_Pressed != 0;

		mt::vec2f point;
		_input->correctPoint_( _event->m_x, _event->m_y, point );

        _input->setTouch_( 0, point, isDown );

        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onMouseButtonEvent( 0, point, 0, isDown );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeInput::s_pointerMotionEvent( s3ePointerMotionEvent * _event, MarmaladeInput * _input )
    {
        TouchDesc desc;
        if( _input->getTouch_( 0, desc ) == false )
        {
            return;
        }

		mt::vec2f point;
		_input->correctPoint_( _event->m_x, _event->m_y, point );

        mt::vec2f diff = point - desc.point;

        _input->setTouch_( 0, point, desc.active );

        ServiceProviderInterface * serviceProvider = _input->getServiceProvider();

        INPUT_SERVICE(serviceProvider)
            ->onMouseMove( 0, point, diff.x, diff.y );
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
            m_keys[i] = (KeyCode)255;
        }

        m_keys[s3eKeyBack] = Menge::KC_ESCAPE;
        m_keys[s3eKeyMenu] = Menge::KC_MENU;

        m_keys[s3eKeyTab] = Menge::KC_TAB;
        m_keys[s3eKeyShift] = Menge::KC_SHIFT;
        m_keys[s3eKeyControl] = Menge::KC_CONTROL;
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
    }
    //////////////////////////////////////////////////////////////////////////
    KeyCode MarmaladeInput::getKeyCode_( s3eKey _key ) const
    {
        if( _key >= 0xFF )
        {
            return KC_UNASSIGNED;
        }

        KeyCode code = m_keys[_key];

        if( code >= 0xFF )
        {
            return KC_UNASSIGNED;
        }
        
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