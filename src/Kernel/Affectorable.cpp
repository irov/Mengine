#include "Affectorable.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affectorable::Affectorable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affectorable::~Affectorable()
    {
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
            AffectorHubInterfacePtr affectorHub = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "BaseAffectorHub" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            AffectorHubProviderInterface * provider = const_cast<Affectorable *>(this)->getAffectorHubProvider();

            affectorHub->setAffectorHubProvider( provider );

            m_affectorHub = affectorHub;
        }

        return m_affectorHub;
    }
}
