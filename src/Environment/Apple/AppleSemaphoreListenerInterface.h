#pragma once

#include "Interface/ServantInterface.h"

#import "Environment/Apple/AppleIncluder.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AppleSemaphoreListenerInterface
        : public ServantInterface
    {
    public:
        virtual void invoke( id _value ) = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSemaphoreListenerInterface> AppleSemaphoreListenerInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}