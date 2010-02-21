#	pragma once

#	include "Factory.h"

#	include <map>

namespace Menge
{
	class Factory;

	class FactoryManager
	{
	public:
		FactoryManager();
		~FactoryManager();

	public:
		void registerFactory( const String & _type, Factory * _factory );
		void unregisterFactory( const String & _type );

	public:
		Factorable * createObject( const String & _type );

		template<class T>
		T * createObjectT( const String & _type )
		{
			return static_cast<T*>( createObject(_type ) );
		}

	public:
		void destroyObject( const String & _type, Factorable * _object );

	protected:
		typedef std::map<String, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
