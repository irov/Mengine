#   include "SDLInput.h"

#	include "Interface/UnicodeInterface.h"

#	include "Logger/Logger.h"

#	include "Config/String.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLInput::SDLInput()
        : m_width(0.f)
        , m_height(0.f)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLInput::~SDLInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLInput::initialize()
    {
        this->fillKeys_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::calcCursorPosition_( int _mx, int _my, mt::vec2f & _point ) const
    {
        float x = static_cast<float>(_mx);
        float y = static_cast<float>(_my);
        
        _point.x = x / m_width;
        _point.y = y / m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::handleEvent( const SDL_Event & _event )
    {
        switch( _event.type )
        {
        case SDL_MOUSEWHEEL:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.wheel.x, _event.wheel.y, point );

                INPUT_SERVICE( m_serviceProvider )
                    ->pushMouseWheelEvent( point.x, point.y, 0, _event.wheel.y );
            }break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            {
                int x;
                int y;
                SDL_GetMouseState( &x, &y );

                mt::vec2f point;
                this->calcCursorPosition_( x, y, point );

                KeyCode code = this->getKeyCode_( _event.key.keysym.scancode );

				if( code == SDL_SCANCODE_UNKNOWN )
				{
					return;
				}

                bool isDown = _event.key.type == SDL_KEYDOWN;
				
                m_keyDown[code] = isDown;
				
                INPUT_SERVICE( m_serviceProvider )
                    ->pushKeyEvent( point.x, point.y, code, isDown, false );
            } break;
		case SDL_TEXTINPUT:
			{
				int x;
				int y;
				SDL_GetMouseState( &x, &y );

				mt::vec2f point;
				this->calcCursorPosition_( x, y, point );

                WChar text_code[8] = { 0 };
				size_t text_code_size;
                UNICODE_SYSTEM( m_serviceProvider )
					->utf8ToUnicode( _event.text.text, (size_t)-1, text_code, 8, &text_code_size );

				INPUT_SERVICE( m_serviceProvider )
					->pushTextEvent( point.x, point.y, text_code[0] );
			}break;
        case SDL_MOUSEMOTION:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.motion.x, _event.motion.y, point );

                mt::vec2f delta;
                this->calcCursorPosition_( _event.motion.xrel, _event.motion.yrel, delta );

                INPUT_SERVICE( m_serviceProvider )
                    ->pushMouseMoveEvent( 0, point.x, point.y, delta.x, delta.y, 0.f );
            }break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.button.x, _event.button.y, point );

                uint32_t button;
                
                switch( _event.button.button )
                {
                case SDL_BUTTON_LEFT: 
                    button = 0; 
                    break;
                case SDL_BUTTON_RIGHT:
                    button = 1; 
                    break;
                case SDL_BUTTON_MIDDLE:
                    button = 2; 
                    break;
                default:
                    button = _event.button.button - 1;
                    break;
                };

                INPUT_SERVICE( m_serviceProvider )
                    ->pushMouseButtonEvent( 0, point.x, point.y, button, 0.f, _event.button.type == SDL_MOUSEBUTTONDOWN );
            }
            break;
        case SDL_FINGERMOTION:
            {
                uint32_t touchId = (uint32_t)_event.tfinger.touchId - 1;

                INPUT_SERVICE( m_serviceProvider )
                    ->pushMouseMoveEvent( touchId, _event.tfinger.x, _event.tfinger.y, _event.tfinger.dx, _event.tfinger.dy, _event.tfinger.pressure );
            }break;
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
            {
                uint32_t touchId = (uint32_t)_event.tfinger.touchId - 1;

                INPUT_SERVICE( m_serviceProvider )
                    ->pushMouseButtonEvent( touchId, _event.tfinger.x, _event.tfinger.y, 0, _event.tfinger.pressure, _event.tfinger.type == SDL_FINGERDOWN );
            }
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::updateSurfaceResolution( float _width, float _height )
    {
        m_width = _width;
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////    
    void SDLInput::showKeyboard( bool _value )
    {
        (void)_value;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLInput::isKeyDown( Menge::KeyCode _code ) const
    {
        return m_keyDown[_code];
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLInput::isAnyKeyDown() const
    {
        for( uint32_t i = 0; i != KC_MAX_CODE; ++i )
        {
            if( m_keyDown[i] == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::fillKeys_()
    {
        for( uint32_t i = 0; i != KC_MAX_CODE; ++i )
        {
            m_keys[i] = KC_UNASSIGNED;

            m_keyDown[i] = false;
        }

        for( uint32_t i = 0; i != SDL_NUM_SCANCODES; ++i )
        {
            m_codes[i] = SDL_SCANCODE_UNKNOWN;
        }				

        m_keys[SDL_SCANCODE_RETURN] = Menge::KC_RETURN;
        m_keys[SDL_SCANCODE_ESCAPE] = Menge::KC_ESCAPE;
        m_keys[SDL_SCANCODE_BACKSPACE] = Menge::KC_BACK;
        m_keys[SDL_SCANCODE_TAB] = Menge::KC_TAB;
        m_keys[SDL_SCANCODE_SPACE] = Menge::KC_SPACE;
                    
        m_keys[SDL_SCANCODE_MENU] = Menge::KC_MENU;

        
        m_keys[SDL_SCANCODE_LSHIFT] = Menge::KC_LSHIFT;
        m_keys[SDL_SCANCODE_RSHIFT] = Menge::KC_RSHIFT;
        m_keys[SDL_SCANCODE_LCTRL] = Menge::KC_LCONTROL;
        m_keys[SDL_SCANCODE_RCTRL] = Menge::KC_RCONTROL;
        m_keys[SDL_SCANCODE_LALT] = Menge::KC_LMENU;
        m_keys[SDL_SCANCODE_RALT] = Menge::KC_RMENU;
        m_keys[SDL_SCANCODE_PAUSE] = Menge::KC_PAUSE;
        m_keys[SDL_SCANCODE_CAPSLOCK] = Menge::KC_CAPITAL;
        
        m_keys[SDL_SCANCODE_PAGEUP] = Menge::KC_PRIOR;
        m_keys[SDL_SCANCODE_PAGEDOWN] = Menge::KC_NEXT;
        m_keys[SDL_SCANCODE_END] = Menge::KC_END;
        m_keys[SDL_SCANCODE_HOME] = Menge::KC_HOME;
        m_keys[SDL_SCANCODE_LEFT] = Menge::KC_LEFT;
        m_keys[SDL_SCANCODE_UP] = Menge::KC_UP;
        m_keys[SDL_SCANCODE_RIGHT] = Menge::KC_RIGHT;
        m_keys[SDL_SCANCODE_DOWN] = Menge::KC_DOWN;
        m_keys[SDL_SCANCODE_INSERT] = Menge::KC_INSERT;
        m_keys[SDL_SCANCODE_DELETE] = Menge::KC_DELETE;

        m_keys[SDL_SCANCODE_0] = Menge::KC_0;
        m_keys[SDL_SCANCODE_1] = Menge::KC_1;
        m_keys[SDL_SCANCODE_2] = Menge::KC_2;
        m_keys[SDL_SCANCODE_3] = Menge::KC_3;
        m_keys[SDL_SCANCODE_4] = Menge::KC_4;
        m_keys[SDL_SCANCODE_5] = Menge::KC_5;
        m_keys[SDL_SCANCODE_6] = Menge::KC_6;
        m_keys[SDL_SCANCODE_7] = Menge::KC_7;
        m_keys[SDL_SCANCODE_8] = Menge::KC_8;
        m_keys[SDL_SCANCODE_9] = Menge::KC_9;

        m_keys[SDL_SCANCODE_A] = Menge::KC_A;
        m_keys[SDL_SCANCODE_B] = Menge::KC_B;
        m_keys[SDL_SCANCODE_C] = Menge::KC_C;
        m_keys[SDL_SCANCODE_D] = Menge::KC_D;
        m_keys[SDL_SCANCODE_E] = Menge::KC_E;
        m_keys[SDL_SCANCODE_F] = Menge::KC_F;
        m_keys[SDL_SCANCODE_G] = Menge::KC_G;
        m_keys[SDL_SCANCODE_H] = Menge::KC_H;
        m_keys[SDL_SCANCODE_I] = Menge::KC_I;
        m_keys[SDL_SCANCODE_J] = Menge::KC_J;
        m_keys[SDL_SCANCODE_K] = Menge::KC_K;
        m_keys[SDL_SCANCODE_L] = Menge::KC_L;
        m_keys[SDL_SCANCODE_M] = Menge::KC_M;
        m_keys[SDL_SCANCODE_N] = Menge::KC_N;
        m_keys[SDL_SCANCODE_O] = Menge::KC_O;
        m_keys[SDL_SCANCODE_P] = Menge::KC_P;
        m_keys[SDL_SCANCODE_Q] = Menge::KC_Q;
        m_keys[SDL_SCANCODE_R] = Menge::KC_R;
        m_keys[SDL_SCANCODE_S] = Menge::KC_S;
        m_keys[SDL_SCANCODE_T] = Menge::KC_T;
        m_keys[SDL_SCANCODE_U] = Menge::KC_U;
        m_keys[SDL_SCANCODE_V] = Menge::KC_V;
        m_keys[SDL_SCANCODE_W] = Menge::KC_W;
        m_keys[SDL_SCANCODE_X] = Menge::KC_X;
        m_keys[SDL_SCANCODE_Y] = Menge::KC_Y;
        m_keys[SDL_SCANCODE_Z] = Menge::KC_Z;

        m_keys[SDL_SCANCODE_KP_1] = Menge::KC_NUMPAD0;
        m_keys[SDL_SCANCODE_KP_2] = Menge::KC_NUMPAD1;
        m_keys[SDL_SCANCODE_KP_3] = Menge::KC_NUMPAD2;
        m_keys[SDL_SCANCODE_KP_4] = Menge::KC_NUMPAD3;
        m_keys[SDL_SCANCODE_KP_5] = Menge::KC_NUMPAD4;
        m_keys[SDL_SCANCODE_KP_6] = Menge::KC_NUMPAD5;
        m_keys[SDL_SCANCODE_KP_7] = Menge::KC_NUMPAD6;
        m_keys[SDL_SCANCODE_KP_8] = Menge::KC_NUMPAD7;
        m_keys[SDL_SCANCODE_KP_9] = Menge::KC_NUMPAD8;
        m_keys[SDL_SCANCODE_KP_0] = Menge::KC_NUMPAD9;

        m_keys[SDL_SCANCODE_KP_MINUS] = Menge::KC_SUBTRACT;
        m_keys[SDL_SCANCODE_KP_PLUS] = Menge::KC_ADD;

        m_keys[SDL_SCANCODE_KP_DIVIDE] = Menge::KC_DIVIDE;

        m_keys[SDL_SCANCODE_F1] = Menge::KC_F1;
        m_keys[SDL_SCANCODE_F2] = Menge::KC_F2;
        m_keys[SDL_SCANCODE_F3] = Menge::KC_F3;
        m_keys[SDL_SCANCODE_F4] = Menge::KC_F4;
        m_keys[SDL_SCANCODE_F5] = Menge::KC_F5;
        m_keys[SDL_SCANCODE_F6] = Menge::KC_F6;
        m_keys[SDL_SCANCODE_F7] = Menge::KC_F7;
        m_keys[SDL_SCANCODE_F8] = Menge::KC_F8;
        m_keys[SDL_SCANCODE_F9] = Menge::KC_F9;
        m_keys[SDL_SCANCODE_F10] = Menge::KC_F10;
        m_keys[SDL_SCANCODE_F11] = Menge::KC_F11;
        m_keys[SDL_SCANCODE_F12] = Menge::KC_F12;

        m_keys[SDL_SCANCODE_LEFTBRACKET] = Menge::KC_OEM_4;
        m_keys[SDL_SCANCODE_RIGHTBRACKET] = Menge::KC_OEM_6;
        m_keys[SDL_SCANCODE_EQUALS] = Menge::KC_OEM_PLUS;
        m_keys[SDL_SCANCODE_MINUS] = Menge::KC_OEM_MINUS;

        m_keys[SDL_SCANCODE_NUMLOCKCLEAR] = Menge::KC_NUMLOCK;

        for( uint32_t i = 0; i != KC_MAX_CODE; ++i )
        {
            KeyCode code = m_keys[i];

            if( code == KC_UNASSIGNED )
            {
                continue;
            }

            m_codes[code] = (SDL_Scancode)i;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    KeyCode SDLInput::getKeyCode_( SDL_Scancode _key ) const
    {
        if( _key >= SDL_NUM_SCANCODES )
        {
            return KC_UNASSIGNED;
        }

        KeyCode code = m_keys[_key];
        
        return code; 
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_Scancode SDLInput::getSDLKey_( KeyCode _code ) const
    {
        if( _code >= KC_MAX_CODE )
        {
            return SDL_SCANCODE_UNKNOWN;
        }

        SDL_Scancode key = m_codes[_code];

        return key; 
    }
}
