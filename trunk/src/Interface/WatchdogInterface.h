#   pragma once

#   include "Config/Typedef.h"

namespace Menge
{
    class WatchdogInterface
    {
    public:
        virtual float watch( const String & _tag ) = 0;
        virtual void destroy() = 0;
    };
}