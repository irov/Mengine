#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class EasingServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "EasingService" )
    };
}
//////////////////////////////////////////////////////////////////////////
#define EASING_SERVICE()\
    ((Mengine::EasingServiceInterface*)SERVICE_GET(Mengine::EasingServiceInterface))
//////////////////////////////////////////////////////////////////////////