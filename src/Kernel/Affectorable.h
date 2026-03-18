#pragma once

#include "Interface/AffectorHubInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Affectorable
        : public AffectorHubProviderInterface
    {
    public:
        Affectorable();
        ~Affectorable() override;

    public:
        void addAffector( const AffectorPtr & _affector );
        void stopAffectors( EAffectorType _type );
        void stopAllAffectors();

    public:
        void setLinearSpeed( const mt::vec3f & _linearSpeed );
        const mt::vec3f & getLinearSpeed() const;

        void setAngularSpeed( float _angular );
        float getAngularSpeed() const;

    public:
        bool availableAffectorHub() const;
        void clearAffectorHub();

    public:
        const AffectorHubInterfacePtr & getAffectorHub() const;

    public:
        mutable AffectorHubInterfacePtr m_affectorHub;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affectorable> AffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}
