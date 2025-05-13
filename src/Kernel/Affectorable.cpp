#include "Affectorable.h"

#include "Kernel/Assertion.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/BaseAffectorHub.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affectorable::Affectorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affectorable::~Affectorable()
    {
        MENGINE_ASSERTION_FATAL( m_affectorHub == nullptr, "affector hub not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affectorable::availableAffectorHub() const
    {
        return m_affectorHub != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::clearAffectorHub()
    {
        m_affectorHub = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorHubInterfacePtr & Affectorable::getAffectorHub() const
    {
        if( m_affectorHub == nullptr )
        {
            AffectorHubInterfacePtr affectorHub = Helper::generatePrototype<BaseAffectorHub>( ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            AffectorHubProviderInterface * provider = const_cast<Affectorable *>(this)->getAffectorHubProvider();

            affectorHub->setAffectorHubProvider( provider );

            m_affectorHub = affectorHub;
        }

        return m_affectorHub;
    }
    //////////////////////////////////////////////////////////////////////////
}
