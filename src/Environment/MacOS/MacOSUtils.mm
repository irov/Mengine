#include "MacOSUtils.h"

#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

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
        bool MacOSSetDesktopWallpaper( const char * _path )
        {
            NSString* urlString = [NSString stringWithUTF8String:_path];
            
            if( urlString == nil )
            {
                return false;
            }
            
	        NSURL * url = [NSURL fileURLWithPath:urlString];

            NSScreen * mainScreen = [NSScreen mainScreen];
    
            if( mainScreen == nil )
            {
                return false;
            }
    	
            NSDictionary * screenOptions = [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:mainScreen];
    
            if( screenOptions == nil )
            {
                return false;
            }

            NSError *error = nil;
            if( [[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                        forScreen:mainScreen
                                                          options:screenOptions
                                                            error:&error] == FALSE )
            {
                LOGGER_ERROR("invalid set desktop image url '%s' error '%s'"
                    , _path
                    , Helper::AppleGetMessageFromNSError(error).c_str()
                );
                
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
