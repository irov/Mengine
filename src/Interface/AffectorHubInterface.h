#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Affector.h"
#include "Kernel/AffectorType.h"
#include "Kernel/Mixin.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/IntrusiveSlugList.h"
#include "Kernel/IntrusiveSlugIterator.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AffectorHubProviderInterface
        : public Mixin
    {
    public:
        virtual EUpdateMode getAffectorableUpdatableMode() const = 0;
        virtual uint32_t getAffectorableUpdatableLeafDeep() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorHubProviderInterface> AffectorHubProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AffectorHubInterface
        : public ServantInterface
    {
    public:
        virtual void setAffectorHubProvider( const AffectorHubProviderInterface * _provider ) = 0;
        virtual const AffectorHubProviderInterface * getAffectorHubProvider() const = 0;

    public:
        virtual void addAffector( const AffectorPtr & _affector ) = 0;

    public:
        virtual void stopAffectors( EAffectorType _type ) = 0;

    public:
        virtual void stopAllAffectors() = 0;

    public:
        virtual void setAngularSpeed( float _angular ) = 0;
        virtual float getAngularSpeed() const = 0;

        virtual void setLinearSpeed( const mt::vec3f & _linearSpeed ) = 0;
        virtual const mt::vec3f & getLinearSpeed() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorHubInterface> AffectorHubInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
