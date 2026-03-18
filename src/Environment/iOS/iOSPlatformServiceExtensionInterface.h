#pragma once

#include "Interface/UnknownInterface.h"

#import <UIKit/UIKit.h>

#include "Config/UniqueId.h"

namespace Mengine
{
    class iOSPlatformServiceExtensionInterface
        : virtual public UnknownInterface
    {
    public:
        virtual UIWindow * getUIWindow() const = 0;

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    public:
        virtual EAGLContext * getEAGLContext() const = 0;
#endif

    public:
        virtual void handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
    };
}
