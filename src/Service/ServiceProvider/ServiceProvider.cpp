#	include "ServiceProvider.h"

#   include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_FACTORY( ServiceProvider, Menge::ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ServiceProvider::ServiceProvider()
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			strcpy( desc.name, "" );
			desc.service = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProvider::~ServiceProvider()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::initializeService( TServiceProviderGenerator _generator )
	{
		if( _generator == nullptr )
		{
			return false;
		}

		ServiceInterface * service;
		if( (*_generator)(&service) == false )
		{
			return false;
		}

		const char * name = service->getServiceID();

		if( strlen( name ) + 1 > SERVICE_PROVIDER_NAME_SIZE )
		{
			return false;
		}

		service->setServiceProvider( this );

		if( service->initialize() == false )
		{
			return false;
		}

		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				strcpy( desc.name, name );
			}
			else if( strcmp( desc.name, name ) != 0 )
			{
				continue;
			}
			else
			{
				desc.service->finalize();
				desc.service->destroy();
			}

			desc.service = service;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::finalizeService( const char * _name )
	{
		for( size_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				break;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			desc.service->finalize();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::destroyService( const char * _name )
	{
		for( size_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				break;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			desc.service->finalize();
			desc.service->destroy();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::existService( const char * _name ) const
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			const ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				continue;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceInterface * ServiceProvider::getService( const char * _name ) const
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			const ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				continue;
			}

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			return desc.service;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void ServiceProvider::destroy()
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( desc.service == nullptr )
			{
				continue;
			}

			desc.service->finalize();
			desc.service->destroy();
			desc.service = nullptr;
		}
	}
}