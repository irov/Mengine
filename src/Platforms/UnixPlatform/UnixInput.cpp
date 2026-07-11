#include "UnixInput.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UnixInput::UnixInput()
        : m_display( nullptr )
        , m_window( 0 )
        , m_windowWidth( 1 )
        , m_windowHeight( 1 )
        , m_cursorX( 0 )
        , m_cursorY( 0 )
    {
        StdAlgorithm::fill_n( m_keyDown, MENGINE_INPUT_MAX_KEY_CODE, false );
        StdAlgorithm::fill_n( m_mouseButtonDown, MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, false );
    }
    //////////////////////////////////////////////////////////////////////////
    UnixInput::~UnixInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixInput::initialize( Display * _display, Window _window )
    {
        m_display = _display;
        m_window = _window;

        this->updateWindowSize_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::finalize()
    {
        m_display = nullptr;
        m_window = 0;

        StdAlgorithm::fill_n( m_keyDown, MENGINE_INPUT_MAX_KEY_CODE, false );
        StdAlgorithm::fill_n( m_mouseButtonDown, MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, false );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp UnixInput::getEventTimestamp_() const
    {
        return Helper::getPlatformTimestamp();
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::updateWindowSize_()
    {
        if( m_display == nullptr || m_window == 0 )
        {
            m_windowWidth = 1;
            m_windowHeight = 1;

            return;
        }

        XWindowAttributes attributes;
        if( ::XGetWindowAttributes( m_display, m_window, &attributes ) == 0 )
        {
            return;
        }

        m_windowWidth = attributes.width > 0 ? (uint32_t)attributes.width : 1U;
        m_windowHeight = attributes.height > 0 ? (uint32_t)attributes.height : 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::updateCursorPosition_( int _x, int _y, mt::vec2f * const _point )
    {
        m_cursorX = _x;
        m_cursorY = _y;

        const float width = (float)(m_windowWidth > 0 ? m_windowWidth : 1U);
        const float height = (float)(m_windowHeight > 0 ? m_windowHeight : 1U);

        _point->x = (float)_x / width;
        _point->y = (float)_y / height;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::getCursorPosition( mt::vec2f * const _point ) const
    {
        const float width = (float)(m_windowWidth > 0 ? m_windowWidth : 1U);
        const float height = (float)(m_windowHeight > 0 ? m_windowHeight : 1U);

        _point->x = (float)m_cursorX / width;
        _point->y = (float)m_cursorY / height;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixInput::getMouseButton_( unsigned int _button, EMouseButtonCode * const _code ) const
    {
        switch( _button )
        {
        case Button1:
            *_code = MC_LBUTTON;
            return true;
        case Button2:
            *_code = MC_MBUTTON;
            return true;
        case Button3:
            *_code = MC_RBUTTON;
            return true;
        case 8:
            *_code = MC_X1BUTTON;
            return true;
        case 9:
            *_code = MC_X2BUTTON;
            return true;
        default:
            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::handleEvent( const XEvent & _event, const Char * _text )
    {
        const Timestamp timestamp = this->getEventTimestamp_();

        switch( _event.type )
        {
        case ConfigureNotify:
            m_windowWidth = _event.xconfigure.width > 0 ? (uint32_t)_event.xconfigure.width : 1U;
            m_windowHeight = _event.xconfigure.height > 0 ? (uint32_t)_event.xconfigure.height : 1U;
            break;
        case MotionNotify:
            {
                const int previousX = m_cursorX;
                const int previousY = m_cursorY;

                mt::vec2f point;
                this->updateCursorPosition_( _event.xmotion.x, _event.xmotion.y, &point );

                const float dx = (float)(m_cursorX - previousX) / (float)m_windowWidth;
                const float dy = (float)(m_cursorY - previousY) / (float)m_windowHeight;

                Helper::pushMouseMoveEvent( timestamp, TC_TOUCH0, point.x, point.y, dx, dy, 0.f, 0.f );
            }break;
        case EnterNotify:
            {
                mt::vec2f point;
                this->updateCursorPosition_( _event.xcrossing.x, _event.xcrossing.y, &point );

                Helper::pushMouseEnterEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );
            }break;
        case LeaveNotify:
            {
                mt::vec2f point;
                this->updateCursorPosition_( _event.xcrossing.x, _event.xcrossing.y, &point );

                Helper::pushMouseLeaveEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );
            }break;
        case ButtonPress:
        case ButtonRelease:
            {
                mt::vec2f point;
                this->updateCursorPosition_( _event.xbutton.x, _event.xbutton.y, &point );

                if( _event.type == ButtonPress && (_event.xbutton.button == Button4 || _event.xbutton.button == Button5) )
                {
                    const int32_t scroll = _event.xbutton.button == Button4 ? 1 : -1;
                    Helper::pushMouseWheelEvent( timestamp, point.x, point.y, 0.f, WC_CENTRAL, scroll );

                    break;
                }

                EMouseButtonCode button;
                if( this->getMouseButton_( _event.xbutton.button, &button ) == false )
                {
                    break;
                }

                const bool down = _event.type == ButtonPress;

                if( down == false && m_mouseButtonDown[button] == false )
                {
                    break;
                }

                m_mouseButtonDown[button] = down;

                Helper::pushMouseButtonEvent( timestamp, TC_TOUCH0, point.x, point.y, button, 0.f, down );
            }break;
        case KeyPress:
        case KeyRelease:
            {
                XKeyEvent keyEvent = _event.xkey;
                KeySym keySym = ::XkbKeycodeToKeysym( m_display, (KeyCode)keyEvent.keycode, 0, 0 );
                EKeyCode code = this->getKeyCode_( keyEvent.keycode, keySym );

                if( code != KC_UNASSIGNED )
                {
                    mt::vec2f point;
                    this->getCursorPosition( &point );

                    const bool down = _event.type == KeyPress;
                    const bool repeating = down == true && m_keyDown[code] == true;

                    if( down == false && m_keyDown[code] == false )
                    {
                        break;
                    }

                    m_keyDown[code] = down;

                    Helper::pushKeyEvent( timestamp, point.x, point.y, 0.f, code, down, repeating );
                }

                if( _event.type != KeyPress || _text == nullptr || (_event.xkey.state & (ControlMask | Mod1Mask | Mod4Mask)) != 0 )
                {
                    break;
                }

                WChar unicode[256 + 1] = {L'\0'};
                size_t unicodeSize = 0;
                if( Helper::utf8ToUnicode( _text, StdString::strlen( _text ), unicode, 256, &unicodeSize ) == false || unicodeSize == 0 )
                {
                    break;
                }

                mt::vec2f point;
                this->getCursorPosition( &point );

                Helper::pushTextEvent( timestamp, point.x, point.y, 0.f, unicode );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixInput::releaseAll()
    {
        const Timestamp timestamp = this->getEventTimestamp_();

        mt::vec2f point;
        this->getCursorPosition( &point );

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_KEY_CODE; ++index )
        {
            if( m_keyDown[index] == false )
            {
                continue;
            }

            m_keyDown[index] = false;
            Helper::pushKeyEvent( timestamp, point.x, point.y, 0.f, (EKeyCode)index, false, false );
        }

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE; ++index )
        {
            if( m_mouseButtonDown[index] == false )
            {
                continue;
            }

            m_mouseButtonDown[index] = false;
            Helper::pushMouseButtonEvent( timestamp, TC_TOUCH0, point.x, point.y, (EMouseButtonCode)index, 0.f, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixInput::isKeyDown( EKeyCode _code ) const
    {
        return m_keyDown[_code];
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixInput::isAnyKeyDown() const
    {
        for( bool down : m_keyDown )
        {
            if( down == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode UnixInput::getKeyCode_( unsigned int _keycode, KeySym _keysym ) const
    {
        EKeyCode code = this->getPhysicalKeyCode_( _keycode );

        if( code != KC_UNASSIGNED )
        {
            return code;
        }

        return this->getKeySymCode_( _keysym );
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode UnixInput::getPhysicalKeyCode_( unsigned int _keycode ) const
    {
        switch( _keycode )
        {
        case 9: return KC_ESCAPE;
        case 10: return KC_1;
        case 11: return KC_2;
        case 12: return KC_3;
        case 13: return KC_4;
        case 14: return KC_5;
        case 15: return KC_6;
        case 16: return KC_7;
        case 17: return KC_8;
        case 18: return KC_9;
        case 19: return KC_0;
        case 20: return KC_OEM_MINUS;
        case 21: return KC_OEM_PLUS;
        case 22: return KC_BACK;
        case 23: return KC_TAB;
        case 24: return KC_Q;
        case 25: return KC_W;
        case 26: return KC_E;
        case 27: return KC_R;
        case 28: return KC_T;
        case 29: return KC_Y;
        case 30: return KC_U;
        case 31: return KC_I;
        case 32: return KC_O;
        case 33: return KC_P;
        case 34: return KC_OEM_4;
        case 35: return KC_OEM_6;
        case 36: return KC_RETURN;
        case 37: return KC_LCONTROL;
        case 38: return KC_A;
        case 39: return KC_S;
        case 40: return KC_D;
        case 41: return KC_F;
        case 42: return KC_G;
        case 43: return KC_H;
        case 44: return KC_J;
        case 45: return KC_K;
        case 46: return KC_L;
        case 47: return KC_OEM_1;
        case 48: return KC_OEM_7;
        case 49: return KC_OEM_3;
        case 50: return KC_LSHIFT;
        case 51: return KC_OEM_5;
        case 52: return KC_Z;
        case 53: return KC_X;
        case 54: return KC_C;
        case 55: return KC_V;
        case 56: return KC_B;
        case 57: return KC_N;
        case 58: return KC_M;
        case 59: return KC_OEM_COMMA;
        case 60: return KC_OEM_PERIOD;
        case 61: return KC_OEM_2;
        case 62: return KC_RSHIFT;
        case 63: return KC_MULTIPLY;
        case 64: return KC_LMENU;
        case 65: return KC_SPACE;
        case 66: return KC_CAPITAL;
        case 67: return KC_F1;
        case 68: return KC_F2;
        case 69: return KC_F3;
        case 70: return KC_F4;
        case 71: return KC_F5;
        case 72: return KC_F6;
        case 73: return KC_F7;
        case 74: return KC_F8;
        case 75: return KC_F9;
        case 76: return KC_F10;
        case 77: return KC_NUMLOCK;
        case 78: return KC_SCROLL;
        case 79: return KC_NUMPAD7;
        case 80: return KC_NUMPAD8;
        case 81: return KC_NUMPAD9;
        case 82: return KC_SUBTRACT;
        case 83: return KC_NUMPAD4;
        case 84: return KC_NUMPAD5;
        case 85: return KC_NUMPAD6;
        case 86: return KC_ADD;
        case 87: return KC_NUMPAD1;
        case 88: return KC_NUMPAD2;
        case 89: return KC_NUMPAD3;
        case 90: return KC_NUMPAD0;
        case 91: return KC_DECIMAL;
        case 95: return KC_F11;
        case 96: return KC_F12;
        case 104: return KC_RETURN;
        case 105: return KC_RCONTROL;
        case 106: return KC_DIVIDE;
        case 107: return KC_SNAPSHOT;
        case 108: return KC_RMENU;
        case 110: return KC_HOME;
        case 111: return KC_UP;
        case 112: return KC_PRIOR;
        case 113: return KC_LEFT;
        case 114: return KC_RIGHT;
        case 115: return KC_END;
        case 116: return KC_DOWN;
        case 117: return KC_NEXT;
        case 118: return KC_INSERT;
        case 119: return KC_DELETE;
        case 127: return KC_PAUSE;
        case 133: return KC_LWIN;
        case 134: return KC_RWIN;
        case 135: return KC_APPS;
        default: return KC_UNASSIGNED;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode UnixInput::getKeySymCode_( KeySym _keysym ) const
    {
        if( _keysym >= XK_0 && _keysym <= XK_9 )
        {
            return (EKeyCode)(KC_0 + (_keysym - XK_0));
        }

        if( _keysym >= XK_A && _keysym <= XK_Z )
        {
            return (EKeyCode)(KC_A + (_keysym - XK_A));
        }

        if( _keysym >= XK_a && _keysym <= XK_z )
        {
            return (EKeyCode)(KC_A + (_keysym - XK_a));
        }

        switch( _keysym )
        {
        case XK_Return: return KC_RETURN;
        case XK_Escape: return KC_ESCAPE;
        case XK_BackSpace: return KC_BACK;
        case XK_Tab: return KC_TAB;
        case XK_space: return KC_SPACE;
        case XK_Delete: return KC_DELETE;
        case XK_Insert: return KC_INSERT;
        case XK_Home: return KC_HOME;
        case XK_End: return KC_END;
        case XK_Page_Up: return KC_PRIOR;
        case XK_Page_Down: return KC_NEXT;
        case XK_Left: return KC_LEFT;
        case XK_Right: return KC_RIGHT;
        case XK_Up: return KC_UP;
        case XK_Down: return KC_DOWN;
        case XK_Shift_L: return KC_LSHIFT;
        case XK_Shift_R: return KC_RSHIFT;
        case XK_Control_L: return KC_LCONTROL;
        case XK_Control_R: return KC_RCONTROL;
        case XK_Alt_L: return KC_LMENU;
        case XK_Alt_R: return KC_RMENU;
        case XK_Super_L: return KC_LWIN;
        case XK_Super_R: return KC_RWIN;
        case XK_Menu: return KC_APPS;
        case XK_Caps_Lock: return KC_CAPITAL;
        case XK_Num_Lock: return KC_NUMLOCK;
        case XK_Scroll_Lock: return KC_SCROLL;
        case XK_KP_0: return KC_NUMPAD0;
        case XK_KP_1: return KC_NUMPAD1;
        case XK_KP_2: return KC_NUMPAD2;
        case XK_KP_3: return KC_NUMPAD3;
        case XK_KP_4: return KC_NUMPAD4;
        case XK_KP_5: return KC_NUMPAD5;
        case XK_KP_6: return KC_NUMPAD6;
        case XK_KP_7: return KC_NUMPAD7;
        case XK_KP_8: return KC_NUMPAD8;
        case XK_KP_9: return KC_NUMPAD9;
        case XK_KP_Multiply: return KC_MULTIPLY;
        case XK_KP_Add: return KC_ADD;
        case XK_KP_Subtract: return KC_SUBTRACT;
        case XK_KP_Decimal: return KC_DECIMAL;
        case XK_KP_Divide: return KC_DIVIDE;
        case XK_F1: return KC_F1;
        case XK_F2: return KC_F2;
        case XK_F3: return KC_F3;
        case XK_F4: return KC_F4;
        case XK_F5: return KC_F5;
        case XK_F6: return KC_F6;
        case XK_F7: return KC_F7;
        case XK_F8: return KC_F8;
        case XK_F9: return KC_F9;
        case XK_F10: return KC_F10;
        case XK_F11: return KC_F11;
        case XK_F12: return KC_F12;
        default: return KC_UNASSIGNED;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
