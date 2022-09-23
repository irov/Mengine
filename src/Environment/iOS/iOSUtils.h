#pragma once

#include "Config/Config.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool iOSGetDeviceId( Char * const _deviceId );
        bool iOSGetDeviceName( Char * const _deviceName );
    }
}
