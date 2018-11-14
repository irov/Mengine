#pragma once

#include "ResourceAstralax.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AstralaxResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceAstralax, bool>
    {
    public:
        AstralaxResourceValidateVisitor();
        ~AstralaxResourceValidateVisitor() override;

    protected:
        bool accept( ResourceAstralax * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxResourceValidateVisitor> AstralaxResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}