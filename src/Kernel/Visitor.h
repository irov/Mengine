#	pragma once

namespace Menge
{
	class Visitor
	{
	public:
		virtual ~Visitor(){}
	};

	template<class T>
	class ConcreteVisitor
		//: public Visitor
	{
	public:
		virtual void accept( T * _visited ) = 0;
	};
}