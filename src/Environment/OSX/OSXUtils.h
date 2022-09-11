#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        bool OSXGetBuildShortVersionString( Char * const _directory );
        bool OSXGetBuildVersionKey( Char * const _directory );
        bool OSXGetMusicDirectory( Char * const _directory );
        bool OSXGetPicturesDirectory( Char * const _directory );
        bool OSXSetDesktopWallpaper( const Char * _url );
    }
}
