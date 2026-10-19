#pragma once

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

#import <Metal/Metal.h>

namespace Mengine
{
    class iOSRenderPlatformServiceExtensionInterface
        : public iOSPlatformServiceExtensionInterface
    {
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
        virtual UIView * getMetalView() const = 0;

    public:
        typedef Lambda<bool( NSSet<UITouch *> * _touches, UIView * _view, UITouchPhase _phase )> LambdaIOSTouchHandler;

        virtual UniqueId addIOSTouchHandler( const LambdaIOSTouchHandler & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeIOSTouchHandler( UniqueId _id ) = 0;

    public:
        virtual void handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
    };
}
