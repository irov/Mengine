#include "ServiceProvider.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_FACTORY( ServiceProvider, Mengine::ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
    ServiceInterfacePtr ServiceProvider::generateService( TServiceProviderGenerator _generator )
    {
        if( _generator == nullptr )
        {
            return nullptr;
        }

        ServiceInterfacePtr service;
        if( (*_generator)(&service) == false )
        {
            return nullptr;
        }

        if( service->initialize() == false )
        {
            return nullptr;
        }

        return service;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::initializeService( TServiceProviderGenerator _generator )
	{
		ServiceInterfacePtr service = this->generateService( _generator );
		
        if( service == nullptr )
		{
			return false;
		}

		const char * name = service->getServiceID();

		if( strlen( name ) + 1 > SERVICE_PROVIDER_NAME_SIZE )
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
				desc.service = nullptr;
			}

			desc.service = service;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::finalizeService( const char * _name )
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			if( desc.service == nullptr )
			{
				break;
			}

			desc.service->finalize();

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::destroyService( const char * _name )
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			ServiceDesc & desc = m_services[index];

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			if( desc.service == nullptr )
			{
				break;
			}

			desc.service->finalize();
			desc.service = nullptr;

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
	const ServiceInterfacePtr & ServiceProvider::getService( const char * _name ) const
	{
		for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
		{
			const ServiceDesc & desc = m_services[index];

			if( strcmp( desc.name, _name ) != 0 )
			{
				continue;
			}

			if( desc.service == nullptr )
			{
				break;
			}

			return desc.service;
		}

		return ServiceInterfacePtr::none();
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
		}

        for( uint32_t index = SERVICE_PROVIDER_COUNT; index != 0; --index )
        {
            ServiceDesc & desc = m_services[index - 1];

            if( desc.service == nullptr )
            {
                continue;
            }

            desc.service = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void ServiceProvider::stopServices()
    {
        for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service == nullptr )
            {
                continue;
            }

            desc.service->stopService();
        }
    }
}