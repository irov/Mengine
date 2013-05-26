#	pragma once

#	include "Interface/PluginInterface.h"

#	include <vector>
#	include <map>

namespace Menge
{
    const size_t SERVICE_PROVIDER_SIZE = 64;

	class ServiceProvider
		: public ServiceProviderInterface
	{
    public:
        ServiceProvider();
        ~ServiceProvider();

    public:
        ServiceInterface * getService( const char * _name ) const override;

	public:
		bool registryService( const char * _name, ServiceInterface * _service ) override;
		bool unregistryService( const char * _name ) override;
        
	protected:
        struct ServiceDesc
        {
            char name[64];
            ServiceInterface * service;
        };

        ServiceDesc m_services[SERVICE_PROVIDER_SIZE];
	};
}