#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AppleSemaphoreListenerInterface
        : public ServantInterface
    {
    public:
        virtual void invoke() = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSemaphoreListenerInterface> AppleSemaphoreListenerInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}