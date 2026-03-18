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
    void Affectorable::addAffector( const AffectorPtr & _affector )
    {
        const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

        affectorHub->addAffector( _affector );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::stopAffectors( EAffectorType _type )
    {
        if( m_affectorHub == nullptr )
        {
            return;
        }

        m_affectorHub->stopAffectors( _type );
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::stopAllAffectors()
    {
        if( m_affectorHub == nullptr )
        {
            return;
        }

        m_affectorHub->stopAllAffectors();
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::setLinearSpeed( const mt::vec3f & _linearSpeed )
    {
        const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

        affectorHub->setLinearSpeed( _linearSpeed );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Affectorable::getLinearSpeed() const
    {
        const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

        return affectorHub->getLinearSpeed();
    }
    //////////////////////////////////////////////////////////////////////////
    void Affectorable::setAngularSpeed( float _angular )
    {
        const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

        affectorHub->setAngularSpeed( _angular );
    }
    //////////////////////////////////////////////////////////////////////////
    float Affectorable::getAngularSpeed() const
    {
        const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

        return affectorHub->getAngularSpeed();
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

            affectorHub->setAffectorHubProvider( this );

            m_affectorHub = affectorHub;
        }

        return m_affectorHub;
    }
    //////////////////////////////////////////////////////////////////////////
}
