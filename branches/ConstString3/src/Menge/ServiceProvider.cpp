#	include "ServiceProvider.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void ServiceProvider::registryService( const String & _name, ServiceInterface * _service )
	{
		m_services.insert( std::make_pair(_name, _service) );

		TMapListenerServices::iterator it_found = m_listeners.find( _name );

		if( it_found != m_listeners.end() )
		{
			const TVectorListenerServices & listeners = it_found->second;

			for( TVectorListenerServices::const_iterator
				it = listeners.begin(),
				it_end = listeners.end();
			it != it_end;
			++it )
			{
				(*it)->onRegistryService( this, _service );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ServiceProvider::unregistryService( const String & _name )
	{
		m_services.erase( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceInterface * ServiceProvider::getService( const String & _name )
	{
		TMapServices::iterator it_found = m_services.find( _name );

		if( it_found == m_services.end() )
		{
			return 0;
		}

		return it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void ServiceProvider::addServiceListener( const String & _name, ServiceListenerInterface * _serviceListener )
	{
		TMapListenerServices::iterator it_found = m_listeners.find( _name );

		if( it_found == m_listeners.end() )
		{
			TVectorListenerServices listeners;
			it_found = m_listeners.insert( std::make_pair(_name, listeners) ).first;
		}

		it_found->second.push_back( _serviceListener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::removeServiceListener( const String & _name, ServiceListenerInterface * _serviceListener )
	{
		TMapListenerServices::iterator it_found = m_listeners.find( _name );

		if( it_found == m_listeners.end() )
		{
			MENGE_LOG_WARNING("ServiceProvider::removeServiceListener service %s not found"
				, _name.c_str()
				);

			return false;
		}

		TVectorListenerServices & listeners = it_found->second;

		for( TVectorListenerServices::iterator
			it = listeners.begin(),
			it_end = listeners.end();
		it != it_end;
		++it )
		{
			if( *it == _serviceListener )
			{
				listeners.erase( it );

				break;
			}
		}

		return true;
	}
}