#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AndroidFunctorVoidInterface
        : public ServantInterface
    {
    public:
        virtual void invoke() = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidFunctorVoidInterface> AndroidFunctorVoidInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}