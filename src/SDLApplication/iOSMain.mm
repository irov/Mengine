#import "SDLUIApplicationDelegate.h"

#import <UIKit/UIKit.h>

int main( int argc, char * argv[] ) {
    @autoreleasepool {
        int result = UIApplicationMain(argc, argv, nil, NSStringFromClass([SDLUIApplicationDelegate class]));
        
        return result;
    }
}
