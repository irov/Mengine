#	include "FactoryManager.h"

#	include "Factory.h"
#	include "Factorable.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FactoryManager::FactoryManager()
        : m_serviceProvider(NULL)
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
    void FactoryManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::registerFactory( const ConstString & _type, Factory * _factory )
	{
        LOGGER_INFO(m_serviceProvider)("FactoryManager::registerFactory: register factory '%s'"
            , _type.c_str() 
            );

		m_factories.insert( std::make_pair(_type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FactoryManager::unregisterFactory( const ConstString & _type )
	{
        LOGGER_INFO(m_serviceProvider)("FactoryManager::unregisterFactory: unregister factory '%s'"
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

			return 0;
		}

        Factory * factory = it_found->second;

		Factorable * object = factory->createObject();

		if( object == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)("FactoryManager::createObject: invalid create object factory '%s'"
                , _type.c_str() 
                );

			return NULL;
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
			_visit->visit( it->first, it->second );
		}
	}
}
