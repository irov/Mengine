#	include "FactoryManager.h"

#	include "Factory.h"
#	include "Factorable.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FactoryManager::FactoryManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FactoryManager::~FactoryManager()
	{
		for( TMapFactory::iterator
			it = m_factories.begin(),
			it_end = m_factories.end();
		it != it_end;
		++it )
		{
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::registerFactory( const ConstString & _type, Factory * _factory )
	{
		MENGE_LOG_INFO("FactoryManager::registerFactory: register factory '%s'"
			, _type.c_str() 
			);

		m_factories.insert( std::make_pair(_type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::unregisterFactory( const ConstString & _type )
	{
		MENGE_LOG_INFO("FactoryManager::unregisterFactory: unregister factory '%s'"
			, _type.c_str() 
			);

		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found != m_factories.end() )
		{
			delete it_found->second;

			m_factories.erase( _type );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * FactoryManager::createObject( const ConstString & _type )
	{
		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found == m_factories.end() )
		{
			MENGE_LOG_INFO("FactoryManager::createObject: not registry factory '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		Factorable * object = 
			it_found->second->createObject( _type );

		if( object == NULL )
		{
			MENGE_LOG_INFO("FactoryManager::createObject: invalid create object factory '%s'"
				, _type.c_str() 
				);

			return NULL;
		}

		object->create();

		return object;
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::visitFactories( VisitorFactoryManager * _visit )
	{
		for( TMapFactory::iterator
			it = m_factories.begin(),
			it_end = m_factories.end();
		it != it_end;
		++it )
		{
			_visit->visit( it->first, it->second );
		}
	}
}
