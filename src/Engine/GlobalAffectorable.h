#pragma once

#include "Factory/FactorableUnique.h"
#include "Factory/Factorable.h"

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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveDerivedPtr<GlobalAffectorable, Affectorable> GlobalAffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}