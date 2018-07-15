#pragma once

#include "Kernel/Visitor.h"

namespace Mengine
{    
	class Visitable
	{
	public:
		virtual void visit( Visitor * _visitor ) = 0;

	protected:
		template<class T>
		static bool s_concreteVisit( T * _visited, Visitor * _visitor )
		{
			typedef ConcreteVisitor<T> TConcreteVisitor;

			TConcreteVisitor * ptr = dynamic_cast<TConcreteVisitor*>(_visitor);
			
			if( ptr == nullptr )
			{
				return false;
			}

			ptr->accept( _visited );

			return true;
		}
	};

#define DECLARE_VISITABLE_BASE() \
public:\
	void visit( Visitor * _visitor ) override { Visitable::s_concreteVisit(this, _visitor); }\
protected:

#define DECLARE_VISITABLE( Base ) \
public:\
	void visit( Visitor * _visitor ) override { if( Visitable::s_concreteVisit(this, _visitor) == false ){ Base::visit(_visitor); } }\
protected:
}