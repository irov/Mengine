#	include "FactoryManager.h"
#	include "Factory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::registerFactory( const String & _type, Factory * _factory )
	{
		m_factories.insert( std::make_pair(_type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::unregisterFactory( const String & _type )
	{
		m_factories.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * FactoryManager::createObject( const String & _type )
	{
		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found == m_factories.end() )
		{
			return 0;
		}

		Factorable * object = 
			it_found->second->createObject();

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::destroyObject( const String & _type, Factorable * _object )
	{
		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found == m_factories.end() )
		{
			return;
		}

		it_found->second->destroyObject( _object );
	}
}