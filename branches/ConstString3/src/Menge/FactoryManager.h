#	pragma once

#	include "Factory.h"

#	include "ConstManager.h"

#	include <map>

namespace Menge
{
	class Factory;
	class ConstString;

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
			return static_cast<T*>( createObject(_type ) );
		}

	public:
		void destroyObject( const ConstString & _type, Factorable * _object );

	protected:
		typedef std::map<ConstString, Factory *> TMapFactory;
		TMapFactory m_factories;
	};
}
