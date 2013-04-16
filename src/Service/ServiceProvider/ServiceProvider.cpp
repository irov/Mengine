#	include "ServiceProvider.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ServiceProvider, Menge::ServiceProviderInterface, Menge::ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::registryService( const String & _name, ServiceInterface * _service )
	{
        if( _service == NULL )
        {
            return false;
        }

		m_services[_name] = _service;

        _service->setServiceProvider( this );

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
				if( (*it)->onRegistryService( this, _service ) == false )
                {
                    return false;
                }
			}
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::unregistryService( const String & _name )
	{
        TMapServices::iterator it_service_found = m_services.find( _name );

        if( it_service_found == m_services.end() )
        {
            return false;
        }

        ServiceInterface * service = it_service_found->second;

        TMapListenerServices::iterator it_listener_found = m_listeners.find( _name );

        if( it_listener_found != m_listeners.end() )
        {
            const TVectorListenerServices & listeners = it_listener_found->second;

            for( TVectorListenerServices::const_iterator
                it = listeners.begin(),
                it_end = listeners.end();
            it != it_end;
            ++it )
            {
                (*it)->onUnregistryService( this, service );
            }
        }

		m_services.erase( it_service_found );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceInterface * ServiceProvider::getService( const String & _name )
	{
		TMapServices::iterator it_found = m_services.find( _name );

		if( it_found == m_services.end() )
		{
			return NULL;
		}

        ServiceInterface * service = it_found->second;

		return service;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::addServiceListener( const String & _name, ServiceListenerInterface * _serviceListener )
	{
        TMapServices::iterator it_service_found = m_services.find( _name );

        if( it_service_found == m_services.end() )
        {
            return false;
        }

		TMapListenerServices::iterator it_found = m_listeners.find( _name );

		if( it_found == m_listeners.end() )
		{
			TVectorListenerServices listeners;
			it_found = m_listeners.insert( std::make_pair(_name, listeners) ).first;
		}

		it_found->second.push_back( _serviceListener );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::removeServiceListener( const String & _name, ServiceListenerInterface * _serviceListener )
	{
		TMapListenerServices::iterator it_found = m_listeners.find( _name );

		if( it_found == m_listeners.end() )
		{
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