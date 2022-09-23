#pragma once

#include "Config/Config.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool iOSGetDeviceID( Char * const _deviceId );
        bool MacOSGetBuildVersionKey( Char * const _directory );
        bool MacOSGetMusicDirectory( Char * const _directory );
        bool MacOSGetPicturesDirectory( Char * const _directory );
        bool MacOSSetDesktopWallpaper( const Char * _path );
    }
}
