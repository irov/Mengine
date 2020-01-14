#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    class EntityBehaviorInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityBehaviorInterface> EntityBehaviorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}