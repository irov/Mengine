#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorFilePath.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BootstrapperInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Bootstrapper" );

    public:
        virtual bool run() = 0;
        virtual void stop() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define BOOTSTRAPPER_SERVICE()\
    ((Mengine::BootstrapperInterface*)SERVICE_GET(Mengine::BootstrapperInterface))
//////////////////////////////////////////////////////////////////////////