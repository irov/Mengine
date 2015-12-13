#	pragma once

#   include "Core/Visitor.h"

namespace Menge
{    
	class Visitable
	{
	public:
		virtual void visit( Visitor * _visitor ) = 0;

	protected:
		template<class T>
		static void s_concreteVisit( T * _visited, Visitor * _visitor )
		{
			typedef ConcreteVisitor<T> TConcreteVisitor;

			TConcreteVisitor * ptr = dynamic_cast<TConcreteVisitor*>(_visitor);
			
			if( ptr == nullptr )
			{
				return;
			}

			ptr->accept( _visited );
		}
	};

#define DECLARE_VISITABLE() \
public:\
	void visit( Visitor * _visitor ) override { return Visitable::s_concreteVisit(this, _visitor); }\
protected:
}