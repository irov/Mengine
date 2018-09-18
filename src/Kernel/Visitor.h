#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Visitor
        : public Mixin
    {
    public:
        virtual ~Visitor() {}
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Visitor> VisitorPtr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ConcreteVisitor
    {
    public:
        virtual void accept( T * _visited ) = 0;
    };
}