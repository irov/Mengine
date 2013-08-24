#	include "FactoryManager.h"

#	include "Factory.h"
#	include "Factorable.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FactoryManager::FactoryManager()
        : m_serviceProvider(nullptr)
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
			Factory * factory = m_factories.get_value(it);

			delete factory;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void FactoryManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::registerFactory( const ConstString & _type, Factory * _factory )
	{
		m_factories.insert( _type, _factory );
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::unregisterFactory( const ConstString & _type )
	{
		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found != m_factories.end() )
		{
			Factory * factory = m_factories.get_value(it_found);

			delete factory;

			m_factories.erase( it_found );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool FactoryManager::hasFactory( const ConstString & _type ) const
	{
		TMapFactory::const_iterator it_found = m_factories.find( _type );

		if( it_found == m_factories.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * FactoryManager::createObject( const ConstString & _type )
	{
		TMapFactory::iterator it_found = m_factories.find( _type );

		if( it_found == m_factories.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("FactoryManager::createObject: not registry factory '%s'"
                , _type.c_str() 
                );

			return nullptr;
		}

        Factory * factory = m_factories.get_value(it_found);

		Factorable * object = factory->createObject();

		if( object == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("FactoryManager::createObject: invalid create object factory '%s'"
                , _type.c_str() 
                );

			return nullptr;
		}

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
			const ConstString & key = m_factories.get_key( it );
			Factory * factory = m_factories.get_value( it );

			_visit->visit( key, factory );
		}
	}
}
