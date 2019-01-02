#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class FrameworkInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Framework" );

    public:
        virtual void onFrameworkInitialize() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FRAMEWORK_SERVICE()\
    ((Mengine::FrameworkInterface*)SERVICE_GET(Mengine::FrameworkInterface))
//////////////////////////////////////////////////////////////////////////