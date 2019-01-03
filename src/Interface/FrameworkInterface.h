#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class FrameworkInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Framework" );

    public:
        virtual bool onFrameworkInitialize() = 0;
        virtual void onFrameworkFinalize() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FRAMEWORK_SERVICE()\
    ((Mengine::FrameworkInterface*)SERVICE_GET(Mengine::FrameworkInterface))
//////////////////////////////////////////////////////////////////////////