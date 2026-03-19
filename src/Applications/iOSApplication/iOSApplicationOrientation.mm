#import "iOSApplicationOrientation.h"

#import "Environment/Apple/AppleBundle.h"

@implementation iOSApplicationOrientation

+ (UIInterfaceOrientationMask)getSupportedInterfaceOrientationMask {
    NSArray<NSString *> * orientations = nil;
    
    UIUserInterfaceIdiom userInterfaceIdiom = UIDevice.currentDevice.userInterfaceIdiom;

    if (userInterfaceIdiom == UIUserInterfaceIdiomPad) {
        orientations = [AppleBundle getApplicationConfigValue:@"SupportedInterfaceOrientations~ipad"];
    }

    if (orientations == nil || [orientations count] == 0) {
        orientations = [AppleBundle getApplicationConfigValue:@"SupportedInterfaceOrientations"];
    }

    if (orientations == nil || [orientations count] == 0) {
        return UIInterfaceOrientationMaskAll;
    }

    UIInterfaceOrientationMask mask = 0;

    for (NSString * orientation in orientations) {
        if ([orientation isEqualToString:@"UIInterfaceOrientationPortrait"] == YES) {
            mask |= UIInterfaceOrientationMaskPortrait;
        }
        else if ([orientation isEqualToString:@"UIInterfaceOrientationPortraitUpsideDown"] == YES) {
            mask |= UIInterfaceOrientationMaskPortraitUpsideDown;
        }
        else if ([orientation isEqualToString:@"UIInterfaceOrientationLandscapeLeft"] == YES) {
            mask |= UIInterfaceOrientationMaskLandscapeLeft;
        }
        else if ([orientation isEqualToString:@"UIInterfaceOrientationLandscapeRight"] == YES) {
            mask |= UIInterfaceOrientationMaskLandscapeRight;
        }
    }

    return (mask != 0) ? mask : UIInterfaceOrientationMaskAll;
}

@end