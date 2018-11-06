#pragma once

#include "ResourceAstralax.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ParticleResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceAstralax, bool>
    {
    public:
        ParticleResourceValidateVisitor();
        ~ParticleResourceValidateVisitor() override;

    protected:
        bool accept( ResourceAstralax * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ParticleResourceValidateVisitor> ParticleResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}