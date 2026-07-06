#pragma once

#include "Interface/InputServiceInterface.h"

#include "Kernel/Factorable.h"

#import <AppKit/AppKit.h>

namespace Mengine
{
    class MacOSInput
        : public Factorable
    {
    public:
        MacOSInput();
        ~MacOSInput() override;

    public:
        bool initialize();
        void finalize();

    public:
        void handleEvent( NSView * _view, NSEvent * _event );

    public:
        bool isKeyDown( EKeyCode _code ) const;
        bool isAnyKeyDown() const;

    public:
        void getCursorPosition( NSView * _view, mt::vec2f * const _point ) const;

    protected:
        void fillKeys_();

    protected:
        void calcCursorPosition_( NSView * _view, NSPoint _point, mt::vec2f * const _out ) const;
        void calcCursorDelta_( NSView * _view, CGFloat _dx, CGFloat _dy, mt::vec2f * const _out ) const;

    protected:
        EKeyCode getKeyCode_( unsigned short _key ) const;
        Timestamp getEventTimestamp_( NSEvent * _event ) const;

    protected:
        bool handleKeyEvent_( NSView * _view, NSEvent * _event, bool _down );
        bool handleFlagsChangedEvent_( NSView * _view, NSEvent * _event );
        bool handleTextEvent_( NSView * _view, NSEvent * _event );

    protected:
        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE];
        EKeyCode m_keys[256];
    };

    typedef IntrusivePtr<MacOSInput> MacOSInputPtr;
}
