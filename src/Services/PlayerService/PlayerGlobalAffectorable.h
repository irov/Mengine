#pragma once

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"

#include "Kernel/Affectorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PlayerGlobalAffectorable
        : public Affectorable
        , public Factorable
    {
    public:
        PlayerGlobalAffectorable();
        ~PlayerGlobalAffectorable() override;

    protected:
        EUpdateMode getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PlayerGlobalAffectorable, Affectorable> PlayerGlobalAffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}