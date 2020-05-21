#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ComponentHubInterface;
    //////////////////////////////////////////////////////////////////////////
    class Componentable
        : public Mixin
    {
    public:
        virtual ComponentHubInterface * getComponentHub()
        {
            return nullptr;
        }

        virtual ComponentHubInterface * getComponentHub() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
}