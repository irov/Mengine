#import "MacOSUtils.h"

#import "Environment/Apple/AppleDetail.h"

#import "Kernel/Logger.h"

#import "Config/StdString.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static void showAlert( const Char * _caption, const Char * _message, NSAlertStyle _style )
            {
                NSString * caption = _caption != nullptr ? @(_caption) : @"";
                NSString * message = _message != nullptr ? @(_message) : @"";

                void (^showAlertBlock)(void) = ^{
                    NSAlert * alert = [[NSAlert alloc] init];
                    alert.messageText = caption;
                    alert.informativeText = message;
                    alert.alertStyle = _style;
                    [alert addButtonWithTitle:@"OK"];
                    [alert runModal];
                };

                if( [NSThread isMainThread] == YES )
                {
                    showAlertBlock();
                }
                else
                {
                    dispatch_sync( dispatch_get_main_queue(), showAlertBlock );
                }
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool MacOSGetBuildShortVersionString( Char * const _version )
        {
            NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];

            if( version == nil )
            {
                return false;
            }

            const Char * version_str = [version UTF8String];

            StdString::strcpy_safe( _version, version_str, MENGINE_MAX_PATH );

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

            StdString::strcpy_safe( _version, version_str, MENGINE_MAX_PATH );

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

            StdString::strcpy_safe( _directory, path_str, MENGINE_MAX_PATH );
            StdString::strchrcat_safe( _directory, MENGINE_PATH_FORWARDSLASH, MENGINE_MAX_PATH );

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

            StdString::strcpy_safe( _directory, path_str, MENGINE_MAX_PATH );
            StdString::strchrcat_safe( _directory, MENGINE_PATH_FORWARDSLASH, MENGINE_MAX_PATH );

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
                    , [[AppleDetail getMessageFromNSError:error] UTF8String]
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void MacOSShowAlert( const Char * _caption, const Char * _message )
        {
            Detail::showAlert( _caption, _message, NSAlertStyleInformational );
        }
        //////////////////////////////////////////////////////////////////////////
        void MacOSShowFatalAlert( const Char * _caption, const Char * _message )
        {
            Detail::showAlert( _caption, _message, NSAlertStyleCritical );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
