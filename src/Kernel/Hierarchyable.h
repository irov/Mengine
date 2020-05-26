#pragma once

#include "Interface/HierarchyInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Hierarchyable
        : public Mixin
    {
    public:
        virtual HierarchyInterface * getHierarchyable()
        {
            return nullptr;
        }

        virtual const HierarchyInterface * getHierarchyable() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Hierarchyable> HierarchyablePtr;
    //////////////////////////////////////////////////////////////////////////
}