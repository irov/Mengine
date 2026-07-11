#pragma once

#include "Environment/Unix/UnixIncluder.h"

#include "Interface/InputServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class UnixInput
        : public Factorable
    {
    public:
        UnixInput();
        ~UnixInput() override;

    public:
        bool initialize( Display * _display, Window _window );
        void finalize();

    public:
        void handleEvent( const XEvent & _event, const Char * _text );
        void releaseAll();

    public:
        bool isKeyDown( EKeyCode _code ) const;
        bool isAnyKeyDown() const;
        void getCursorPosition( mt::vec2f * const _point ) const;

    protected:
        EKeyCode getKeyCode_( unsigned int _keycode, KeySym _keysym ) const;
        EKeyCode getPhysicalKeyCode_( unsigned int _keycode ) const;
        EKeyCode getKeySymCode_( KeySym _keysym ) const;
        bool getMouseButton_( unsigned int _button, EMouseButtonCode * const _code ) const;

        void updateWindowSize_();
        void updateCursorPosition_( int _x, int _y, mt::vec2f * const _point );
        Timestamp getEventTimestamp_() const;

    protected:
        Display * m_display;
        Window m_window;

        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE];
        bool m_mouseButtonDown[MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE];

        uint32_t m_windowWidth;
        uint32_t m_windowHeight;
        int m_cursorX;
        int m_cursorY;
    };

    typedef IntrusivePtr<UnixInput> UnixInputPtr;
}
