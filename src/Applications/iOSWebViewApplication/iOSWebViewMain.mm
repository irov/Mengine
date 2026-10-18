#import "iOSWebViewApplicationDelegate.h"

//////////////////////////////////////////////////////////////////////////
int main( int _argc, char ** _argv )
{
    @autoreleasepool
    {
        NSString * delegate = [NSBundle.mainBundle objectForInfoDictionaryKey:@"MengineApplicationDelegate"];

        if( delegate == nil )
        {
            delegate = NSStringFromClass( iOSWebViewApplicationDelegate.class );
        }

        int result = UIApplicationMain( _argc, _argv, nil, delegate );

        return result;
    }
}
//////////////////////////////////////////////////////////////////////////
