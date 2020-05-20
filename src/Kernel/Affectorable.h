#pragma once

#include "Interface/AffectorHubInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Affectorable
        : public Mixin
    {
    public:
        Affectorable();
        ~Affectorable() override;

    public:
        bool availableAffectorHub() const;
        void clearAffectorHub();

    public:
        const AffectorHubInterfacePtr & getAffectorHub() const;

    protected:
        virtual const AffectorHubProviderInterface * getAffectorHubProvider() const = 0;

    public:
        mutable AffectorHubInterfacePtr m_affectorHub;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affectorable> AffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}
