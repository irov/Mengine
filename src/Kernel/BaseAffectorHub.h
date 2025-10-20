#pragma once

#include "Interface/AffectorHubInterface.h"

#include "Kernel/Affector.h"
#include "Kernel/AffectorType.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/IntrusiveSlugList.h"
#include "Kernel/IntrusiveSlugIterator.h"

#include "math/vec3.h"

namespace Mengine
{
    class BaseAffectorHub
        : public AffectorHubInterface
    {
        DECLARE_FACTORABLE( BaseAffectorHub );

    public:
        BaseAffectorHub();
        ~BaseAffectorHub() override;

    public:
        void setAffectorHubProvider( const AffectorHubProviderInterface * _provider ) override final;
        const AffectorHubProviderInterface * getAffectorHubProvider() const override final;

    public:
        void addAffector( const AffectorPtr & _affector ) override final;

    public:
        void stopAffectors( EAffectorType _type ) override final;

    public:
        void stopAllAffectors() override final;

    public:
        void setAngularSpeed( float _angular ) override final;
        float getAngularSpeed() const override final;

        void setLinearSpeed( const mt::vec3f & _linearSpeed ) override final;
        const mt::vec3f & getLinearSpeed() const override final;

    protected:
        const AffectorHubProviderInterface * m_provider;

        typedef IntrusiveSlugList<Affector> IntrusiveSlugListAffector;
        typedef IntrusiveSlugIterator<IntrusiveSlugListAffector> IntrusiveSlugAffector;

        IntrusiveSlugListAffector m_affectors;

        float m_angularSpeed;
        mt::vec3f m_linearSpeed;
    };
}
