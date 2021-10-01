#include "SDLInput.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLInput::SDLInput()
        : m_width( 0.f )
        , m_height( 0.f )
        , m_widthInv( 0.f )
        , m_heightInv( 0.f )
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
    void SDLInput::calcCursorPosition_( Sint32 _mx, Sint32 _my, mt::vec2f * const _point ) const
    {
        float x = static_cast<float>(_mx);
        float y = static_cast<float>(_my);

        _point->x = x * m_widthInv;
        _point->y = y * m_heightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode SDLInput::acquireFingerIndex_( SDL_FingerID _fingerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            SDL_FingerID fingerId = m_fingers[index];

            if( fingerId == -1 )
            {
                m_fingers[index] = _fingerId;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode SDLInput::releaseFingerIndex_( SDL_FingerID _fingerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            SDL_FingerID fingerId = m_fingers[index];

            if( fingerId == _fingerId )
            {
                m_fingers[index] = -1;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode SDLInput::getFingerIndex_( SDL_FingerID _fingerId ) const
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            SDL_FingerID fingerId = m_fingers[index];

            if( fingerId == _fingerId )
            {
                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::handleEvent( const SDL_Event & _event )
    {
        switch( _event.type )
        {
        case SDL_MOUSEWHEEL:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.wheel.x, _event.wheel.y, &point );

                Helper::pushMouseWheelEvent( point.x, point.y, WC_CENTRAL, _event.wheel.y );
            }break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            {
                int x;
                int y;
                Uint32 state = SDL_GetMouseState( &x, &y );

                MENGINE_UNUSED( state );

                mt::vec2f point;
                this->calcCursorPosition_( x, y, &point );

                EKeyCode code = this->getKeyCode_( _event.key.keysym.scancode );

                if( code == KC_UNASSIGNED )
                {
                    return;
                }

                bool isDown = _event.key.type == SDL_KEYDOWN;

                m_keyDown[code] = isDown;

                Helper::pushKeyEvent( point.x, point.y, code, isDown, false );

#ifdef MENGINE_PLATFORM_MOBILE
                if( code == KC_RETURN )
                {
                    SDL_StopTextInput();
                }
#endif
            } break;
        case SDL_TEXTINPUT:
            {
                int x;
                int y;
                Uint32 state = SDL_GetMouseState( &x, &y );

                MENGINE_UNUSED( state );

                mt::vec2f point;
                this->calcCursorPosition_( x, y, &point );

                WChar text_code[8] = {L'\0'};
                size_t text_code_size;
                UNICODE_SYSTEM()
                    ->utf8ToUnicode( _event.text.text, MENGINE_UNKNOWN_SIZE, text_code, 7, &text_code_size );

                Helper::pushTextEvent( point.x, point.y, text_code[0] );
            }break;
        case SDL_MOUSEMOTION:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.motion.x, _event.motion.y, &point );

                mt::vec2f delta;
                this->calcCursorPosition_( _event.motion.xrel, _event.motion.yrel, &delta );

                Helper::pushMouseMoveEvent( TC_TOUCH0, point.x, point.y, delta.x, delta.y, 0.f );
            }break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _event.button.x, _event.button.y, &point );

                EMouseCode code;

                switch( _event.button.button )
                {
                case SDL_BUTTON_LEFT:
                    code = MC_LBUTTON;
                    break;
                case SDL_BUTTON_RIGHT:
                    code = MC_RBUTTON;
                    break;
                case SDL_BUTTON_MIDDLE:
                    code = MC_MBUTTON;
                    break;
                default:
                    code = static_cast<EMouseCode>(_event.button.button - 1);
                    break;
                };

                Helper::pushMouseButtonEvent( TC_TOUCH0, point.x, point.y, code, 0.f, _event.button.type == SDL_MOUSEBUTTONDOWN );
            }
            break;
        case SDL_FINGERMOTION:
            {
                ETouchCode fingerIndex = this->getFingerIndex_( _event.tfinger.fingerId );

                Helper::pushMouseMoveEvent( fingerIndex, _event.tfinger.x, _event.tfinger.y, _event.tfinger.dx, _event.tfinger.dy, _event.tfinger.pressure );
            }break;
        case SDL_FINGERDOWN:
            {
                ETouchCode fingerIndex = this->acquireFingerIndex_( _event.tfinger.fingerId );

                Helper::pushMouseButtonEvent( fingerIndex, _event.tfinger.x, _event.tfinger.y, MC_LBUTTON, _event.tfinger.pressure, true );
            }break;
        case SDL_FINGERUP:
            {
                ETouchCode fingerIndex = this->releaseFingerIndex_( _event.tfinger.fingerId );

                Helper::pushMouseButtonEvent( fingerIndex, _event.tfinger.x, _event.tfinger.y, MC_LBUTTON, _event.tfinger.pressure, false );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::updateSurfaceResolution( float _width, float _height )
    {
        m_width = _width;
        m_height = _height;

        m_widthInv = 1.f / m_width;
        m_heightInv = 1.f / m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLInput::isKeyDown( EKeyCode _code ) const
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
    void SDLInput::getCursorPosition( mt::vec2f * const _point ) const
    {
        int x;
        int y;
        Uint32 state = SDL_GetMouseState( &x, &y );

        MENGINE_UNUSED( state );

        this->calcCursorPosition_( x, y, _point );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLInput::fillKeys_()
    {
        m_keys[SDL_SCANCODE_RETURN] = Mengine::KC_RETURN;
        m_keys[SDL_SCANCODE_ESCAPE] = Mengine::KC_ESCAPE;
        m_keys[SDL_SCANCODE_BACKSPACE] = Mengine::KC_BACK;
        m_keys[SDL_SCANCODE_TAB] = Mengine::KC_TAB;
        m_keys[SDL_SCANCODE_SPACE] = Mengine::KC_SPACE;

        m_keys[SDL_SCANCODE_MENU] = Mengine::KC_MENU;


        m_keys[SDL_SCANCODE_LSHIFT] = Mengine::KC_LSHIFT;
        m_keys[SDL_SCANCODE_RSHIFT] = Mengine::KC_RSHIFT;
        m_keys[SDL_SCANCODE_LCTRL] = Mengine::KC_LCONTROL;
        m_keys[SDL_SCANCODE_RCTRL] = Mengine::KC_RCONTROL;
        m_keys[SDL_SCANCODE_LALT] = Mengine::KC_LMENU;
        m_keys[SDL_SCANCODE_RALT] = Mengine::KC_RMENU;
        m_keys[SDL_SCANCODE_PAUSE] = Mengine::KC_PAUSE;
        m_keys[SDL_SCANCODE_CAPSLOCK] = Mengine::KC_CAPITAL;

        m_keys[SDL_SCANCODE_PAGEUP] = Mengine::KC_PRIOR;
        m_keys[SDL_SCANCODE_PAGEDOWN] = Mengine::KC_NEXT;
        m_keys[SDL_SCANCODE_END] = Mengine::KC_END;
        m_keys[SDL_SCANCODE_HOME] = Mengine::KC_HOME;
        m_keys[SDL_SCANCODE_LEFT] = Mengine::KC_LEFT;
        m_keys[SDL_SCANCODE_UP] = Mengine::KC_UP;
        m_keys[SDL_SCANCODE_RIGHT] = Mengine::KC_RIGHT;
        m_keys[SDL_SCANCODE_DOWN] = Mengine::KC_DOWN;
        m_keys[SDL_SCANCODE_INSERT] = Mengine::KC_INSERT;
        m_keys[SDL_SCANCODE_DELETE] = Mengine::KC_DELETE;

        m_keys[SDL_SCANCODE_0] = Mengine::KC_0;
        m_keys[SDL_SCANCODE_1] = Mengine::KC_1;
        m_keys[SDL_SCANCODE_2] = Mengine::KC_2;
        m_keys[SDL_SCANCODE_3] = Mengine::KC_3;
        m_keys[SDL_SCANCODE_4] = Mengine::KC_4;
        m_keys[SDL_SCANCODE_5] = Mengine::KC_5;
        m_keys[SDL_SCANCODE_6] = Mengine::KC_6;
        m_keys[SDL_SCANCODE_7] = Mengine::KC_7;
        m_keys[SDL_SCANCODE_8] = Mengine::KC_8;
        m_keys[SDL_SCANCODE_9] = Mengine::KC_9;

        m_keys[SDL_SCANCODE_A] = Mengine::KC_A;
        m_keys[SDL_SCANCODE_B] = Mengine::KC_B;
        m_keys[SDL_SCANCODE_C] = Mengine::KC_C;
        m_keys[SDL_SCANCODE_D] = Mengine::KC_D;
        m_keys[SDL_SCANCODE_E] = Mengine::KC_E;
        m_keys[SDL_SCANCODE_F] = Mengine::KC_F;
        m_keys[SDL_SCANCODE_G] = Mengine::KC_G;
        m_keys[SDL_SCANCODE_H] = Mengine::KC_H;
        m_keys[SDL_SCANCODE_I] = Mengine::KC_I;
        m_keys[SDL_SCANCODE_J] = Mengine::KC_J;
        m_keys[SDL_SCANCODE_K] = Mengine::KC_K;
        m_keys[SDL_SCANCODE_L] = Mengine::KC_L;
        m_keys[SDL_SCANCODE_M] = Mengine::KC_M;
        m_keys[SDL_SCANCODE_N] = Mengine::KC_N;
        m_keys[SDL_SCANCODE_O] = Mengine::KC_O;
        m_keys[SDL_SCANCODE_P] = Mengine::KC_P;
        m_keys[SDL_SCANCODE_Q] = Mengine::KC_Q;
        m_keys[SDL_SCANCODE_R] = Mengine::KC_R;
        m_keys[SDL_SCANCODE_S] = Mengine::KC_S;
        m_keys[SDL_SCANCODE_T] = Mengine::KC_T;
        m_keys[SDL_SCANCODE_U] = Mengine::KC_U;
        m_keys[SDL_SCANCODE_V] = Mengine::KC_V;
        m_keys[SDL_SCANCODE_W] = Mengine::KC_W;
        m_keys[SDL_SCANCODE_X] = Mengine::KC_X;
        m_keys[SDL_SCANCODE_Y] = Mengine::KC_Y;
        m_keys[SDL_SCANCODE_Z] = Mengine::KC_Z;

        m_keys[SDL_SCANCODE_KP_1] = Mengine::KC_NUMPAD0;
        m_keys[SDL_SCANCODE_KP_2] = Mengine::KC_NUMPAD1;
        m_keys[SDL_SCANCODE_KP_3] = Mengine::KC_NUMPAD2;
        m_keys[SDL_SCANCODE_KP_4] = Mengine::KC_NUMPAD3;
        m_keys[SDL_SCANCODE_KP_5] = Mengine::KC_NUMPAD4;
        m_keys[SDL_SCANCODE_KP_6] = Mengine::KC_NUMPAD5;
        m_keys[SDL_SCANCODE_KP_7] = Mengine::KC_NUMPAD6;
        m_keys[SDL_SCANCODE_KP_8] = Mengine::KC_NUMPAD7;
        m_keys[SDL_SCANCODE_KP_9] = Mengine::KC_NUMPAD8;
        m_keys[SDL_SCANCODE_KP_0] = Mengine::KC_NUMPAD9;

        m_keys[SDL_SCANCODE_KP_MINUS] = Mengine::KC_SUBTRACT;
        m_keys[SDL_SCANCODE_KP_PLUS] = Mengine::KC_ADD;

        m_keys[SDL_SCANCODE_KP_DIVIDE] = Mengine::KC_DIVIDE;

        m_keys[SDL_SCANCODE_F1] = Mengine::KC_F1;
        m_keys[SDL_SCANCODE_F2] = Mengine::KC_F2;
        m_keys[SDL_SCANCODE_F3] = Mengine::KC_F3;
        m_keys[SDL_SCANCODE_F4] = Mengine::KC_F4;
        m_keys[SDL_SCANCODE_F5] = Mengine::KC_F5;
        m_keys[SDL_SCANCODE_F6] = Mengine::KC_F6;
        m_keys[SDL_SCANCODE_F7] = Mengine::KC_F7;
        m_keys[SDL_SCANCODE_F8] = Mengine::KC_F8;
        m_keys[SDL_SCANCODE_F9] = Mengine::KC_F9;
        m_keys[SDL_SCANCODE_F10] = Mengine::KC_F10;
        m_keys[SDL_SCANCODE_F11] = Mengine::KC_F11;
        m_keys[SDL_SCANCODE_F12] = Mengine::KC_F12;

        m_keys[SDL_SCANCODE_LEFTBRACKET] = Mengine::KC_OEM_4;
        m_keys[SDL_SCANCODE_RIGHTBRACKET] = Mengine::KC_OEM_6;
        m_keys[SDL_SCANCODE_EQUALS] = Mengine::KC_OEM_PLUS;
        m_keys[SDL_SCANCODE_MINUS] = Mengine::KC_OEM_MINUS;

        m_keys[SDL_SCANCODE_NUMLOCKCLEAR] = Mengine::KC_NUMLOCK;

        for( uint32_t i = 0; i != KC_MAX_CODE; ++i )
        {
            EKeyCode code = m_keys[i];

            if( code == KC_UNASSIGNED )
            {
                continue;
            }

            m_codes[code] = (SDL_Scancode)i;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode SDLInput::getKeyCode_( SDL_Scancode _key ) const
    {
        if( _key >= SDL_NUM_SCANCODES )
        {
            return KC_UNASSIGNED;
        }

        EKeyCode code = m_keys[_key];

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    SDL_Scancode SDLInput::getSDLKey_( EKeyCode _code ) const
    {
        if( _code >= KC_MAX_CODE )
        {
            return SDL_SCANCODE_UNKNOWN;
        }

        SDL_Scancode key = m_codes[_code];

        return key;
    }
    //////////////////////////////////////////////////////////////////////////
}
