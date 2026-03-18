#pragma once

#include "Interface/AffectorHubInterface.h"

#include "Kernel/Affectorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"

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
    typedef IntrusivePtr<PlayerGlobalAffectorable, AffectorHubInterface> PlayerGlobalAffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}