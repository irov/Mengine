#pragma once

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factorable.h"
#include "Kernel/IntrusiveDerivedPtr.h"

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