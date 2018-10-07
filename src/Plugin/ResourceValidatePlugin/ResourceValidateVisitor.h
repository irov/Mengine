#pragma once

#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceImageSequence.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceImageDefault, bool>
        , public ConcreteVisitorR<ResourceImageSequence, bool>
    {
    public:
        ResourceValidateVisitor();
        ~ResourceValidateVisitor() override;

    protected:
        bool accept( ResourceImageDefault * _resource ) override;
        bool accept( ResourceImageSequence * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceValidateVisitor> ResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}