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
    //////////////////////////////////////////////////////////////////////////
    class BaseAffectorHub
        : public AffectorHubInterface
    {
        DECLARE_FACTORABLE( BaseAffectorHub );

    public:
        BaseAffectorHub();
        ~BaseAffectorHub() override;

    public:
        void setAffectorHubProvider( const AffectorHubProviderInterface * _provider ) override;
        const AffectorHubProviderInterface * getAffectorHubProvider() const override;

    public:
        UniqueId addAffector( const AffectorPtr & _affector ) override;
        bool hasAffector( UniqueId _id ) const override;
        bool stopAffector( UniqueId _id ) override;

    public:
        void stopAffectors( EAffectorType _type ) override;

    public:
        void stopAllAffectors() override;

    public:
        void setAngularSpeed( float _angular ) override;
        float getAngularSpeed() const override;

        void setLinearSpeed( const mt::vec3f & _linearSpeed ) override;
        const mt::vec3f & getLinearSpeed() const override;

    protected:
        const AffectorHubProviderInterface * m_provider;

        typedef IntrusiveSlugList<Affector> IntrusiveSlugListAffector;
        typedef IntrusiveSlugIterator<IntrusiveSlugListAffector> IntrusiveSlugAffector;

        IntrusiveSlugListAffector m_affectors;

        float m_angularSpeed;
        mt::vec3f m_linearSpeed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseAffectorHub> BaseAffectorHubPtr;
    //////////////////////////////////////////////////////////////////////////
}
