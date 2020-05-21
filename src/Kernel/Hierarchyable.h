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
        Hierarchyable();
        ~Hierarchyable() override;

    public:
        bool availableHierarchyable() const;
        void clearHierarchyable();

    public:
        const HierarchyInterfacePtr & getHierarchyable() const;

    protected:
        virtual HierarchyReceiverInterface * getHierarchyableReceiver() = 0;

    public:
        mutable HierarchyInterfacePtr m_hierarchy;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Hierarchyable> HierarchyablePtr;
    //////////////////////////////////////////////////////////////////////////
}