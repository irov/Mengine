#import "SDLUIApplicationDelegate.h"

#import <UIKit/UIKit.h>

extern int MENGINE_MAIN_argc;
extern char ** MENGINE_MAIN_argv;

int main( int argc, char * argv[] ) {
    MENGINE_MAIN_argc = argc;
    MENGINE_MAIN_argv = (char **)malloc( (argc + 1) * sizeof(char *) );
    
    for( int i = 0; i != argc; ++i )
    {
        char * arg = argv[i];
        
        size_t arg_len = strlen( arg );
        char * copy_arg = (char *)malloc( arg_len + 1 );
        strcpy( copy_arg, arg );
        
        MENGINE_MAIN_argv[i] = copy_arg;
    }
    
    MENGINE_MAIN_argv[argc] = NULL;
    
    NSLog(@"Launch Megnine application");
    
    int result;
    
    @autoreleasepool {
        result = UIApplicationMain(argc, argv, nil, NSStringFromClass([SDLUIApplicationDelegate class]));
    }
    
    NSLog(@"Finish Megnine application: %d", result);
    
    for( int i = 0; i != argc; ++i )
    {
        char * copy_arg = MENGINE_MAIN_argv[i];
        
        free( copy_arg );
    }
    
    free( MENGINE_MAIN_argv );
    
    MENGINE_MAIN_argc = 0;
    MENGINE_MAIN_argv = nullptr;
    
    return result;
}
