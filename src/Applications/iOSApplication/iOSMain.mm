#import "SDLUIApplicationDelegate.h"

#import <UIKit/UIKit.h>

int main( int argc, char * argv[] ) {
    NSLog(@"🟢 Launch Megnine application");

    int result;
    
    @autoreleasepool {
        @try {
            result = UIApplicationMain(argc, argv, nil, NSStringFromClass([SDLUIApplicationDelegate class]));
        } @catch (NSException * ex) {
            result = EXIT_FAILURE;
            
            NSLog(@"🔴 [ERROR] Exception Megnine application: %@ [%@]", ex.description, ex.reason);
        }
    }
    
    NSLog(@"🟢 Finish Megnine application: %d", result);
    
    return result;
}
