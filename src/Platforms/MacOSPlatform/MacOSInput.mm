#import "MacOSInput.h"

#import "Kernel/InputServiceHelper.h"
#import "Kernel/UnicodeHelper.h"

#import "Config/StdAlgorithm.h"
#import "Config/StdMath.h"

#import <Carbon/Carbon.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSInput::MacOSInput()
    {
        StdAlgorithm::fill_n( m_keyDown, MENGINE_INPUT_MAX_KEY_CODE, false );
        StdAlgorithm::fill_n( m_keys, 256, KC_UNASSIGNED );
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSInput::~MacOSInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::initialize()
    {
        this->fillKeys_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp MacOSInput::getEventTimestamp_( NSEvent * _event ) const
    {
        NSTimeInterval timestamp = [_event timestamp];

        return (Timestamp)(timestamp * 1000.0);
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::calcCursorPosition_( NSView * _view, NSPoint _point, mt::vec2f * const _out ) const
    {
        if( _view == nil )
        {
            _out->x = 0.f;
            _out->y = 0.f;

            return;
        }

        NSPoint viewPoint = [_view convertPoint:_point fromView:nil];
        NSRect bounds = [_view bounds];

        CGFloat width = bounds.size.width;
        CGFloat height = bounds.size.height;

        if( width <= 0.0 || height <= 0.0 )
        {
            _out->x = 0.f;
            _out->y = 0.f;

            return;
        }

        _out->x = (float)(viewPoint.x / width);
        _out->y = (float)(1.0 - viewPoint.y / height);
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::calcCursorDelta_( NSView * _view, CGFloat _dx, CGFloat _dy, mt::vec2f * const _out ) const
    {
        if( _view == nil )
        {
            _out->x = 0.f;
            _out->y = 0.f;

            return;
        }

        NSRect bounds = [_view bounds];

        CGFloat width = bounds.size.width;
        CGFloat height = bounds.size.height;

        if( width <= 0.0 || height <= 0.0 )
        {
            _out->x = 0.f;
            _out->y = 0.f;

            return;
        }

        _out->x = (float)(_dx / width);
        _out->y = (float)(-_dy / height);
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::getCursorPosition( NSView * _view, mt::vec2f * const _point ) const
    {
        if( _view == nil || [_view window] == nil )
        {
            _point->x = 0.f;
            _point->y = 0.f;

            return;
        }

        NSPoint point = [[_view window] mouseLocationOutsideOfEventStream];

        this->calcCursorPosition_( _view, point, _point );
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::handleEvent( NSView * _view, NSEvent * _event )
    {
        NSEventType eventType = [_event type];

        switch( eventType )
        {
        case NSEventTypeScrollWheel:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _view, [_event locationInWindow], &point );

                CGFloat scrollingDeltaY = [_event scrollingDeltaY];

                int32_t scroll = (int32_t)StdMath::round( (float)scrollingDeltaY );

                if( scroll == 0 && scrollingDeltaY != 0.0 )
                {
                    scroll = scrollingDeltaY > 0.0 ? 1 : -1;
                }

                if( scroll != 0 )
                {
                    Helper::pushMouseWheelEvent( this->getEventTimestamp_( _event ), point.x, point.y, 0.f, WC_CENTRAL, scroll );
                }
            }break;
        case NSEventTypeKeyDown:
            {
                if( this->handleKeyEvent_( _view, _event, true ) == true )
                {
                    this->handleTextEvent_( _view, _event );
                }
            }break;
        case NSEventTypeKeyUp:
            {
                this->handleKeyEvent_( _view, _event, false );
            }break;
        case NSEventTypeFlagsChanged:
            {
                this->handleFlagsChangedEvent_( _view, _event );
            }break;
        case NSEventTypeMouseMoved:
        case NSEventTypeLeftMouseDragged:
        case NSEventTypeRightMouseDragged:
        case NSEventTypeOtherMouseDragged:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _view, [_event locationInWindow], &point );

                mt::vec2f delta;
                this->calcCursorDelta_( _view, [_event deltaX], [_event deltaY], &delta );

                Helper::pushMouseMoveEvent( this->getEventTimestamp_( _event ), TC_TOUCH0, point.x, point.y, delta.x, delta.y, 0.f, 0.f );
            }break;
        case NSEventTypeLeftMouseDown:
        case NSEventTypeRightMouseDown:
        case NSEventTypeOtherMouseDown:
        case NSEventTypeLeftMouseUp:
        case NSEventTypeRightMouseUp:
        case NSEventTypeOtherMouseUp:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _view, [_event locationInWindow], &point );

                EMouseButtonCode button = MC_LBUTTON;

                NSInteger buttonNumber = [_event buttonNumber];

                if( buttonNumber == 0 )
                {
                    button = MC_LBUTTON;
                }
                else if( buttonNumber == 1 )
                {
                    button = MC_RBUTTON;
                }
                else if( buttonNumber == 2 )
                {
                    button = MC_MBUTTON;
                }
                else if( buttonNumber == 3 )
                {
                    button = MC_X1BUTTON;
                }
                else if( buttonNumber == 4 )
                {
                    button = MC_X2BUTTON;
                }
                else
                {
                    return;
                }

                bool down = eventType == NSEventTypeLeftMouseDown ||
                    eventType == NSEventTypeRightMouseDown ||
                    eventType == NSEventTypeOtherMouseDown;

                Helper::pushMouseButtonEvent( this->getEventTimestamp_( _event ), TC_TOUCH0, point.x, point.y, button, 0.f, down );
            }break;
        case NSEventTypeMouseEntered:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _view, [_event locationInWindow], &point );

                Helper::pushMouseEnterEvent( this->getEventTimestamp_( _event ), TC_TOUCH0, point.x, point.y, 0.f );
            }break;
        case NSEventTypeMouseExited:
            {
                mt::vec2f point;
                this->calcCursorPosition_( _view, [_event locationInWindow], &point );

                Helper::pushMouseLeaveEvent( this->getEventTimestamp_( _event ), TC_TOUCH0, point.x, point.y, 0.f );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::handleKeyEvent_( NSView * _view, NSEvent * _event, bool _down )
    {
        unsigned short keyCode = [_event keyCode];

        EKeyCode code = this->getKeyCode_( keyCode );

        if( code == KC_UNASSIGNED )
        {
            return false;
        }

        mt::vec2f point;
        this->getCursorPosition( _view, &point );

        m_keyDown[code] = _down;

        Helper::pushKeyEvent( this->getEventTimestamp_( _event ), point.x, point.y, 0.f, code, _down, [_event isARepeat] == YES );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::handleFlagsChangedEvent_( NSView * _view, NSEvent * _event )
    {
        unsigned short keyCode = [_event keyCode];

        EKeyCode code = this->getKeyCode_( keyCode );

        if( code == KC_UNASSIGNED )
        {
            return false;
        }

        NSEventModifierFlags flags = [_event modifierFlags];
        bool down = false;

        switch( code )
        {
        case KC_LSHIFT:
        case KC_RSHIFT:
            down = (flags & NSEventModifierFlagShift) != 0;
            break;
        case KC_LCONTROL:
        case KC_RCONTROL:
            down = (flags & NSEventModifierFlagControl) != 0;
            break;
        case KC_LMENU:
        case KC_RMENU:
            down = (flags & NSEventModifierFlagOption) != 0;
            break;
        case KC_LWIN:
        case KC_RWIN:
            down = (flags & NSEventModifierFlagCommand) != 0;
            break;
        case KC_CAPITAL:
            down = (flags & NSEventModifierFlagCapsLock) != 0;
            break;
        default:
            down = !m_keyDown[code];
            break;
        }

        if( m_keyDown[code] == down )
        {
            return true;
        }

        mt::vec2f point;
        this->getCursorPosition( _view, &point );

        m_keyDown[code] = down;

        Helper::pushKeyEvent( this->getEventTimestamp_( _event ), point.x, point.y, 0.f, code, down, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::handleTextEvent_( NSView * _view, NSEvent * _event )
    {
        NSEventModifierFlags flags = [_event modifierFlags];

        if( (flags & NSEventModifierFlagCommand) != 0 || (flags & NSEventModifierFlagControl) != 0 )
        {
            return false;
        }

        NSString * characters = [_event characters];

        if( characters == nil || [characters length] == 0 )
        {
            return false;
        }

        const Char * text = [characters UTF8String];

        if( text == nullptr || text[0] == '\0' )
        {
            return false;
        }

        WChar textCode[256 + 1] = {L'\0'};
        size_t textCodeSize;
        Helper::utf8ToUnicode( text, textCode, 256, &textCodeSize );

        if( textCodeSize == 0 )
        {
            return false;
        }

        mt::vec2f point;
        this->getCursorPosition( _view, &point );

        Helper::pushTextEvent( this->getEventTimestamp_( _event ), point.x, point.y, 0.f, textCode );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::isKeyDown( EKeyCode _code ) const
    {
        bool down = m_keyDown[_code];

        return down;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSInput::isAnyKeyDown() const
    {
        for( uint32_t i = 0; i != MENGINE_INPUT_MAX_KEY_CODE; ++i )
        {
            if( m_keyDown[i] == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    EKeyCode MacOSInput::getKeyCode_( unsigned short _key ) const
    {
        if( _key >= 256 )
        {
            return KC_UNASSIGNED;
        }

        EKeyCode code = m_keys[_key];

        return code;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSInput::fillKeys_()
    {
        m_keys[kVK_Return] = KC_RETURN;
        m_keys[kVK_Escape] = KC_ESCAPE;
        m_keys[kVK_Delete] = KC_BACK;
        m_keys[kVK_Tab] = KC_TAB;
        m_keys[kVK_Space] = KC_SPACE;
        m_keys[kVK_ForwardDelete] = KC_DELETE;

        m_keys[kVK_Shift] = KC_LSHIFT;
        m_keys[kVK_RightShift] = KC_RSHIFT;
        m_keys[kVK_Control] = KC_LCONTROL;
        m_keys[kVK_RightControl] = KC_RCONTROL;
        m_keys[kVK_Option] = KC_LMENU;
        m_keys[kVK_RightOption] = KC_RMENU;
        m_keys[kVK_Command] = KC_LWIN;
        m_keys[kVK_RightCommand] = KC_RWIN;
        m_keys[kVK_CapsLock] = KC_CAPITAL;

        m_keys[kVK_PageUp] = KC_PRIOR;
        m_keys[kVK_PageDown] = KC_NEXT;
        m_keys[kVK_End] = KC_END;
        m_keys[kVK_Home] = KC_HOME;
        m_keys[kVK_LeftArrow] = KC_LEFT;
        m_keys[kVK_UpArrow] = KC_UP;
        m_keys[kVK_RightArrow] = KC_RIGHT;
        m_keys[kVK_DownArrow] = KC_DOWN;

        m_keys[kVK_ANSI_0] = KC_0;
        m_keys[kVK_ANSI_1] = KC_1;
        m_keys[kVK_ANSI_2] = KC_2;
        m_keys[kVK_ANSI_3] = KC_3;
        m_keys[kVK_ANSI_4] = KC_4;
        m_keys[kVK_ANSI_5] = KC_5;
        m_keys[kVK_ANSI_6] = KC_6;
        m_keys[kVK_ANSI_7] = KC_7;
        m_keys[kVK_ANSI_8] = KC_8;
        m_keys[kVK_ANSI_9] = KC_9;

        m_keys[kVK_ANSI_A] = KC_A;
        m_keys[kVK_ANSI_B] = KC_B;
        m_keys[kVK_ANSI_C] = KC_C;
        m_keys[kVK_ANSI_D] = KC_D;
        m_keys[kVK_ANSI_E] = KC_E;
        m_keys[kVK_ANSI_F] = KC_F;
        m_keys[kVK_ANSI_G] = KC_G;
        m_keys[kVK_ANSI_H] = KC_H;
        m_keys[kVK_ANSI_I] = KC_I;
        m_keys[kVK_ANSI_J] = KC_J;
        m_keys[kVK_ANSI_K] = KC_K;
        m_keys[kVK_ANSI_L] = KC_L;
        m_keys[kVK_ANSI_M] = KC_M;
        m_keys[kVK_ANSI_N] = KC_N;
        m_keys[kVK_ANSI_O] = KC_O;
        m_keys[kVK_ANSI_P] = KC_P;
        m_keys[kVK_ANSI_Q] = KC_Q;
        m_keys[kVK_ANSI_R] = KC_R;
        m_keys[kVK_ANSI_S] = KC_S;
        m_keys[kVK_ANSI_T] = KC_T;
        m_keys[kVK_ANSI_U] = KC_U;
        m_keys[kVK_ANSI_V] = KC_V;
        m_keys[kVK_ANSI_W] = KC_W;
        m_keys[kVK_ANSI_X] = KC_X;
        m_keys[kVK_ANSI_Y] = KC_Y;
        m_keys[kVK_ANSI_Z] = KC_Z;

        m_keys[kVK_ANSI_Keypad0] = KC_NUMPAD0;
        m_keys[kVK_ANSI_Keypad1] = KC_NUMPAD1;
        m_keys[kVK_ANSI_Keypad2] = KC_NUMPAD2;
        m_keys[kVK_ANSI_Keypad3] = KC_NUMPAD3;
        m_keys[kVK_ANSI_Keypad4] = KC_NUMPAD4;
        m_keys[kVK_ANSI_Keypad5] = KC_NUMPAD5;
        m_keys[kVK_ANSI_Keypad6] = KC_NUMPAD6;
        m_keys[kVK_ANSI_Keypad7] = KC_NUMPAD7;
        m_keys[kVK_ANSI_Keypad8] = KC_NUMPAD8;
        m_keys[kVK_ANSI_Keypad9] = KC_NUMPAD9;

        m_keys[kVK_ANSI_KeypadMinus] = KC_SUBTRACT;
        m_keys[kVK_ANSI_KeypadPlus] = KC_ADD;
        m_keys[kVK_ANSI_KeypadMultiply] = KC_MULTIPLY;
        m_keys[kVK_ANSI_KeypadDivide] = KC_DIVIDE;
        m_keys[kVK_ANSI_KeypadDecimal] = KC_DECIMAL;
        m_keys[kVK_ANSI_KeypadEnter] = KC_RETURN;

        m_keys[kVK_F1] = KC_F1;
        m_keys[kVK_F2] = KC_F2;
        m_keys[kVK_F3] = KC_F3;
        m_keys[kVK_F4] = KC_F4;
        m_keys[kVK_F5] = KC_F5;
        m_keys[kVK_F6] = KC_F6;
        m_keys[kVK_F7] = KC_F7;
        m_keys[kVK_F8] = KC_F8;
        m_keys[kVK_F9] = KC_F9;
        m_keys[kVK_F10] = KC_F10;
        m_keys[kVK_F11] = KC_F11;
        m_keys[kVK_F12] = KC_F12;
        m_keys[kVK_F13] = KC_F13;
        m_keys[kVK_F14] = KC_F14;
        m_keys[kVK_F15] = KC_F15;
        m_keys[kVK_F16] = KC_F16;
        m_keys[kVK_F17] = KC_F17;
        m_keys[kVK_F18] = KC_F18;
        m_keys[kVK_F19] = KC_F19;
        m_keys[kVK_F20] = KC_F20;

        m_keys[kVK_ANSI_LeftBracket] = KC_OEM_4;
        m_keys[kVK_ANSI_RightBracket] = KC_OEM_6;
        m_keys[kVK_ANSI_Equal] = KC_OEM_PLUS;
        m_keys[kVK_ANSI_Minus] = KC_OEM_MINUS;
        m_keys[kVK_ANSI_Comma] = KC_OEM_COMMA;
        m_keys[kVK_ANSI_Period] = KC_OEM_PERIOD;
        m_keys[kVK_ANSI_Slash] = KC_OEM_2;
        m_keys[kVK_ANSI_Backslash] = KC_OEM_5;
        m_keys[kVK_ANSI_Semicolon] = KC_OEM_1;
        m_keys[kVK_ANSI_Quote] = KC_OEM_7;
        m_keys[kVK_ANSI_Grave] = KC_OEM_3;
    }
    //////////////////////////////////////////////////////////////////////////
}
