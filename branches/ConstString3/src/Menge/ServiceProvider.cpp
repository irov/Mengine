#	include "ServiceProvider.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void ServiceProvider::registryService( const String & _name, ServiceInterface * _service )
	{
		m_services.insert( std::make_pair(_name, _service) );
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
}