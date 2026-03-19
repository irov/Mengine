#import "iOSViewController.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

#import "Environment/Apple/AppleBundle.h"

@implementation iOSViewController

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    static UIInterfaceOrientationMask cachedMask = 0;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        NSArray<NSString *> * orientations = [AppleBundle getApplicationConfigValue:@"SupportedInterfaceOrientations"];
        
        if( orientations == nil || [orientations count] == 0 )
        {
            cachedMask = UIInterfaceOrientationMaskAll;
            return;
        }
        
        UIInterfaceOrientationMask mask = 0;
        
        for( NSString * orientation in orientations )
        {
            if( [orientation isEqualToString:@"UIInterfaceOrientationPortrait"] == YES )
            {
                mask |= UIInterfaceOrientationMaskPortrait;
            }
            else if( [orientation isEqualToString:@"UIInterfaceOrientationPortraitUpsideDown"] == YES )
            {
                mask |= UIInterfaceOrientationMaskPortraitUpsideDown;
            }
            else if( [orientation isEqualToString:@"UIInterfaceOrientationLandscapeLeft"] == YES )
            {
                mask |= UIInterfaceOrientationMaskLandscapeLeft;
            }
            else if( [orientation isEqualToString:@"UIInterfaceOrientationLandscapeRight"] == YES )
            {
                mask |= UIInterfaceOrientationMaskLandscapeRight;
            }
        }
        
        cachedMask = (mask != 0) ? mask : UIInterfaceOrientationMaskAll;
    });
    
    return cachedMask;
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
