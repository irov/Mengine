#pragma once

#include "Kernel/Visitor.h"
#include "Kernel/Mixin.h"

#include "Config/DynamicCast.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool concreteVisit( T * _visited, Visitor * _visitor )
        {
            typedef ConcreteVisitor<T> TConcreteVisitor;

            TConcreteVisitor * ptr = Helper::dynamicCast<TConcreteVisitor *>( _visitor );

            if( ptr == nullptr )
            {
                return false;
            }

            ptr->accept( _visited );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    class Visitable
        : public Mixin
    {
    public:
        virtual void visit( Visitor * _visitor ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Visitable> VisitablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_VISITABLE_BASE() \
public:\
    void visit( Visitor * _visitor ) override { Mengine::Detail::concreteVisit(this, _visitor); }\
protected:
//////////////////////////////////////////////////////////////////////////
#define DECLARE_VISITABLE( Base ) \
public:\
    void visit( Visitor * _visitor ) override { if( Mengine::Detail::concreteVisit(this, _visitor) == false ){ Base::visit(_visitor); } }\
protected:
//////////////////////////////////////////////////////////////////////////