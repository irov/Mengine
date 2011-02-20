#	pragma once

#	include "Factory.h"

#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class Factory;

	class VisitorFactoryManager
	{
	public:
		virtual void visit( const ConstString & _type, Factory * _factory ) = 0;
	};

	class FactoryManager
	{
	public:
		FactoryManager();
		~FactoryManager();

	public:
		void registerFactory( const ConstString & _type, Factory * _factory );
		void unregisterFactory( const ConstString & _type );

	public:
		Factorable * createObject( const ConstString & _type );

		template<class T>
		T * createObjectT( const ConstString & _type )
		{
			return static_cast<T*>(this->createObject(_type ));
		}

	public:
		void visitFactories( VisitorFactoryManager * _visit );

	protected:
		typedef std::map<ConstString, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
