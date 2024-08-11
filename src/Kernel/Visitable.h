#pragma once

#include "Kernel/Visitor.h"
#include "Kernel/Mixin.h"

#include "Config/DynamicCast.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Visitable
        : public Mixin
    {
    public:
        virtual void visit( const VisitorPtr & _visitor ) = 0;
        virtual bool visitIf( const VisitorPtr & _visitor ) = 0;

    protected:
        template<class T>
        static bool s_concreteVisit( T * _visited, const VisitorPtr & _visitor )
        {
            typedef ConcreteVisitor<T> TConcreteVisitor;

            Visitor * visitor_ptr = _visitor.get();

            TConcreteVisitor * ptr = Helper::dynamicCast<TConcreteVisitor *>( visitor_ptr );

            if( ptr == nullptr )
            {
                return false;
            }

            ptr->accept( _visited );

            return true;
        }

        template<class T>
        static bool s_concreteVisitIf( T * _visited, const VisitorPtr & _visitor, bool * const _result )
        {
            typedef ConcreteVisitorR<T, bool> TConcreteVisitorR;

            Visitor * visitor_ptr = _visitor.get();

            TConcreteVisitorR * ptr = Helper::dynamicCast<TConcreteVisitorR *>( visitor_ptr );

            if( ptr == nullptr )
            {
                return false;
            }

            *_result = ptr->accept( _visited );

            return true;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Visitable> VisitablePtr;
    //////////////////////////////////////////////////////////////////////////
#define DECLARE_VISITABLE_BASE() \
public:\
    void visit( const Mengine::VisitorPtr & _visitor ) override { Mengine::Visitable::s_concreteVisit(this, _visitor); }\
    bool visitIf( const Mengine::VisitorPtr & _visitor ) override { bool result = true; Mengine::Visitable::s_concreteVisitIf(this, _visitor, &result); return result;}\
protected:
    //////////////////////////////////////////////////////////////////////////
#define DECLARE_VISITABLE( Base ) \
public:\
    void visit( const Mengine::VisitorPtr & _visitor ) override { if( Mengine::Visitable::s_concreteVisit(this, _visitor) == false ){ Base::visit(_visitor); } }\
    bool visitIf( const Mengine::VisitorPtr & _visitor ) override { bool result = true; if( Mengine::Visitable::s_concreteVisitIf(this, _visitor, &result) == false ){ result = Base::visitIf(_visitor); } return result;}\
protected:
    //////////////////////////////////////////////////////////////////////////
}