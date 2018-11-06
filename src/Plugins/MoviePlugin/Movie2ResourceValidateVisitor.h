#pragma once

#include "ResourceMovie2.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2ResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceMovie2, bool>
    {
    public:
        Movie2ResourceValidateVisitor();
        ~Movie2ResourceValidateVisitor() override;

    protected:
        bool accept( ResourceMovie2 * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2ResourceValidateVisitor> Movie2ResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}