#include "AppleUtils.h"

#include "Kernel/StringCopy.h"

#include "Config/StdString.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        void AppleLogMessage( const Char * _message )
        {
            NSLog( @"%s", _message );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleLogFormat( const Char * _format, ... )
        {
			MENGINE_VA_LIST_TYPE args;
			MENGINE_VA_LIST_START( args, _format );
            Helper::AppleLogFormatV( _format, args );
			MENGINE_VA_LIST_END( args );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleLogFormatV( const Char * _format, MENGINE_VA_LIST_TYPE _va )
        {
            NSLogv( @(_format), _va );
        }
        /////////////////////////////////////////////////////////////////////////
        bool AppleOpenUrlInDefaultBrowser( const Char * _url )
        {
            NSString * url = [NSString stringWithUTF8String: _url];
    
        #if defined(MENGINE_PLATFORM_MACOS)
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
        #else
            if( [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]] == TRUE )
            {
                [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url] options:@{} completionHandler:^(BOOL success)
                {
                    //ToDo callback
                }];
            }
            else
            {
                return false;
            }
        #endif
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
