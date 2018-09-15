#pragma once

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"

#include "Kernel/Affectorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GlobalAffectorable
        : public FactorableUnique<Factorable>
        , public Affectorable
    {
    public:
        GlobalAffectorable();
        ~GlobalAffectorable() override;

    protected:
        uint32_t getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GlobalAffectorable, Affectorable> GlobalAffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}