#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AndroidSemaphoreListenerInterface
        : public ServantInterface
    {
    public:
        virtual void invoke() = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSemaphoreListenerInterface> AndroidSemaphoreListenerInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}