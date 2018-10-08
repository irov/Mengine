#pragma once

#include "ResourceParticle.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ParticleResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceParticle, bool>
    {
    public:
        ParticleResourceValidateVisitor();
        ~ParticleResourceValidateVisitor() override;

    protected:
        bool accept( ResourceParticle * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ParticleResourceValidateVisitor> ParticleResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}