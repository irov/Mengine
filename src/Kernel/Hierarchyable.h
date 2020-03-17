#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HierarchyInterface;
    //////////////////////////////////////////////////////////////////////////
    class Hierarchyable
        : public Mixin
    {
    public:
        virtual HierarchyInterface * getHierarchy()
        {
            return nullptr;
        }

        virtual const HierarchyInterface * getHierarchy() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Hierarchyable> HierarchyablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_HIERARCHY()\
public:\
    Mengine::HierarchyInterface * getHierarchy() override{ return this; }\
    const Mengine::HierarchyInterface * getHierarchy() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
