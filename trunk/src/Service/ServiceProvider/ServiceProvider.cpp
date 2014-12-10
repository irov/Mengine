#	include "ServiceProvider.h"

#	include "Interface/LogSystemInterface.h"

#   include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ServiceProvider, Menge::ServiceProviderInterface, Menge::ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ServiceProvider::ServiceProvider()
    {
        for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            desc.service = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProvider::~ServiceProvider()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::registryService( const char * _name, ServiceInterface * _service )
	{
        if( _service == nullptr )
        {
            return false;
        }

		if( strlen( _name ) + 1 > SERVICE_PROVIDER_NAME_SIZE )
		{
			return false;
		}

        for( uint32_t index = 0; index != SERVICE_PROVIDER_COUNT; ++index )
        {
            ServiceDesc & desc = m_services[index];

            if( desc.service != nullptr )
            {
                if( strcmp( desc.name, _name ) == 0 )
                {
                    desc.service = _service;
                }

                continue;
            }

            strcpy( desc.name, _name );
            desc.service = _service;

            desc.service->setServiceProvider( this );

            return true;
        }

        return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ServiceProvider::unregistryService( const char * _name )
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
 }