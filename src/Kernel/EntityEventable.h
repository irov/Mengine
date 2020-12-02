#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Entity.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EntityEventable
        : public Factorable
        , public Eventable
        , public BaseEventation
    {
        DECLARE_EVENTABLE();

    public:
        EntityEventable();
        ~EntityEventable() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityEventable> EntityEventablePtr;
    //////////////////////////////////////////////////////////////////////////
}