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
	{
	public:
		virtual void accept( T * _visited ) = 0;
	};
}