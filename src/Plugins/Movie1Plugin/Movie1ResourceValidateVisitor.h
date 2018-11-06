#pragma once

#include "ResourceMovie.h"

#include "Kernel/Visitor.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie1ResourceValidateVisitor
        : public Visitor
        , public Factorable
        , public ConcreteVisitorR<ResourceMovie, bool>
    {
    public:
        Movie1ResourceValidateVisitor();
        ~Movie1ResourceValidateVisitor() override;

    protected:
        bool accept( ResourceMovie * _resource ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie1ResourceValidateVisitor> Movie1ResourceValidateVisitorPtr;
    //////////////////////////////////////////////////////////////////////////
}