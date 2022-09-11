#pragma once

#include "Config/Config.h"
#include "Config/Char.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        bool MacOSGetBuildShortVersionString( Char * const _directory );
        bool MacOSGetBuildVersionKey( Char * const _directory );
        bool MacOSGetMusicDirectory( Char * const _directory );
        bool MacOSGetPicturesDirectory( Char * const _directory );
        bool MacOSSetDesktopWallpaper( const Char * _url );
    }
}
