#include "OSXSetDesktopWallpaper.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <string.h>

//////////////////////////////////////////////////////////////////////////
int OSXGetMusicDirectory( char * const _directory )
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES);
    
    if( paths == nil )
    {
        return -1;
    }
    
    NSString * path = [paths objectAtIndex:0];
    
    if( path == nil )
    {
        return -1;
    }
    
    const char * path_str = [path UTF8String];
    
    strcpy( _directory, path_str );
    strcat( _directory, "/" );
    
    return 0;
}
//////////////////////////////////////////////////////////////////////////
int OSXGetPicturesDirectory( char * const _directory )
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSPicturesDirectory, NSUserDomainMask, YES);
    
    if( paths == nil )
    {
        return -1;
    }
    
    NSString * path = [paths objectAtIndex:0];
    
    if( path == nil )
    {
        return -1;
    }
        
    const char * path_str = [path UTF8String];
        
    strcpy( _directory, path_str );
    strcat( _directory, "/" );
    
    return 0;
}
//////////////////////////////////////////////////////////////////////////
int OSXSetDesktopWallpaper( const char * _url )
{ 
	NSURL * url = [NSURL fileURLWithPath:@(_url)];

    NSScreen * currentScreen = [NSScreen mainScreen];
    
    if( currentScreen == nil )
    {
        return -1;
    }
    	
    NSDictionary * screenOptions = [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:currentScreen];
    
    NSError *error = nil;
    
    if( [[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                forScreen:currentScreen
                                                  options:screenOptions
                                                    error:&error] == FALSE )
    {
        return -1;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////
