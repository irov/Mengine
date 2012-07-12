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
		bool hasFactory( const ConstString & _type ) const;

	public:
		Factorable * createObject( const ConstString & _type );

		template<class T>
		T * createObjectT( const ConstString & _type )
		{
			Factorable * factorable = this->createObject( _type );

			if( dynamic_cast<T*>(factorable) == NULL )
			{
				return NULL;
			}

			T * t = static_cast<T*>(factorable);			

			return t;
		}

	public:
		void visitFactories( VisitorFactoryManager * _visit );

	protected:
		typedef std::map<ConstString, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
