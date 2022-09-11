#include "MacOSUtils.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool MacOSGetBuildShortVersionString( Char * const _version )
        {
            NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    
            if( version == nil )
            {
                return false;
            }
    
            const Char * version_str = [version UTF8String];
    
            MENGINE_STRCPY( _version, version_str );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool MacOSGetBuildVersionKey( Char * const _version )
        {
            NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString *)kCFBundleVersionKey];
    
            if( version == nil )
            {
                return false;
            }
    
            const Char * version_str = [version UTF8String];
    
            MENGINE_STRCPY( _version, version_str );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool MacOSGetMusicDirectory( Char * const _directory )
        {
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES);
    
            if( paths == nil )
            {
                return false;
            }
    
            NSString * path = [paths objectAtIndex:0];
    
            if( path == nil )
            {
                return false;
            }
    
            const Char * path_str = [path UTF8String];
    
            MENGINE_STRCPY( _directory, path_str );
            MENGINE_STRCAT( _directory, "/" );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool MacOSGetPicturesDirectory( Char * const _directory )
        {
            NSArray * paths = NSSearchPathForDirectoriesInDomains(NSPicturesDirectory, NSUserDomainMask, YES);
    
            if( paths == nil )
            {
                return false;
            }
    
            NSString * path = [paths objectAtIndex:0];
    
            if( path == nil )
            {
                return false;
            }
        
            const Char * path_str = [path UTF8String];
        
            MENGINE_STRCPY( _directory, path_str );
            MENGINE_STRCAT( _directory, "/" );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool MacOSSetDesktopWallpaper( const char * _url )
        { 
	        NSURL * url = [NSURL fileURLWithPath:@(_url)];

            NSScreen * currentScreen = [NSScreen mainScreen];
    
            if( currentScreen == nil )
            {
                return false;
            }
    	
            NSDictionary * screenOptions = [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:currentScreen];
    
            if( screenOptions == nil )
            {
                return false;
            }

            NSError *error = nil;
    
            if( [[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                        forScreen:currentScreen
                                                          options:screenOptions
                                                            error:&error] == FALSE )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
