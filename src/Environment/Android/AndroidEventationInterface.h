#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidEventationInterface
        : public ServantInterface
    {
        friend class AndroidEventationHub;

    protected:
        virtual void invoke() = 0;
    };
    ///////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidEventationInterface> AndroidEventationInterfacePtr;
    ///////////////////////////////////////////////////////////////////////
}