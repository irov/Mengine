#import "iOSViewController.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

@implementation iOSViewController

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchBegan( touches, self.view );
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchMoved( touches, self.view );
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchEnded( touches, self.view );
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchCancelled( touches, self.view );
    }
}

@end
