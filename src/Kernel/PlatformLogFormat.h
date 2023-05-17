#pragma once

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void PlatformLogMessage( const Char * _message );
        void PlatformLogFormat( const Char * _format, ... );
    }
}