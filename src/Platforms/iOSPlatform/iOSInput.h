#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputServiceInterface.h"

#include "Kernel/Factorable.h"

#include "math/vec2.h"

#import <UIKit/UIKit.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class iOSInput
        : public Factorable
    {
    public:
        iOSInput();
        ~iOSInput() override;

    public:
        bool initialize();
        void finalize();

    public:
        void handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view );
        void handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view );
        void handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view );
        void handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view );

    public:
        bool isKeyDown( EKeyCode _code ) const;
        bool isAnyKeyDown() const;

    public:
        void getCursorPosition( mt::vec2f * const _point ) const;

    protected:
        void calcCursorPosition_( UIView * _view, CGPoint _location, mt::vec2f * const _point ) const;

    protected:
        ETouchCode acquireFingerIndex_( UITouch * _touch );
        ETouchCode releaseFingerIndex_( UITouch * _touch );
        ETouchCode getFingerIndex_( UITouch * _touch ) const;

    protected:
        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE];

        UITouch * m_fingers[MENGINE_INPUT_MAX_TOUCH];

        mt::vec2f m_lastCursorPosition;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<iOSInput> iOSInputPtr;
    //////////////////////////////////////////////////////////////////////////
}
