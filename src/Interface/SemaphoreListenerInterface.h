#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Params.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SemaphoreListenerInterface
        : public ServantInterface
    {
    public:
        virtual void invoke( const ParamVariant & _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SemaphoreListenerInterface> SemaphoreListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
