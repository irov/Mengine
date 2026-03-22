#import "iOSUIApplicationDelegate.h"

#import <UIKit/UIKit.h>

int main( int argc, char * argv[] ) {
    NSLog(@"🟢 Launch Mengine application");

    int result;
    
    @autoreleasepool {
        @try {
            result = UIApplicationMain(argc, argv, nil, NSStringFromClass([iOSUIApplicationDelegate class]));
        } @catch (NSException * ex) {
            result = EXIT_FAILURE;
            
            NSLog(@"🔴 [ERROR] Exception Mengine application: %@ [%@]", ex.description, ex.reason);
        }
    }
    
    NSLog(@"🟢 Finish Mengine application: %d", result);
    
    return result;
}
