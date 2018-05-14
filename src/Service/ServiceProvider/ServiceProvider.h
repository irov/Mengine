#pragma once

#include "Interface/PluginInterface.h"

namespace Mengine
{
	const uint32_t SERVICE_PROVIDER_NAME_SIZE = 64;
    const uint32_t SERVICE_PROVIDER_COUNT = 64;

	class ServiceProvider
		: public ServiceProviderInterface
	{
    public:
        ServiceProvider();
        ~ServiceProvider() override;

	public:
		bool existService( const char * _name ) const override;

    protected:
        const ServiceInterfacePtr & getService( const char * _name ) const override;

	protected:
        ServiceInterfacePtr generateService( TServiceProviderGenerator _generator ) override;
		bool initializeService( TServiceProviderGenerator _generator ) override;
		bool finalizeService( const char * _name ) override;

		bool destroyService( const char * _name ) override;
		
    protected:
        void stopServices() override;

	protected:
		void destroy() override;

	protected:
        struct ServiceDesc
        {
            char name[SERVICE_PROVIDER_NAME_SIZE];
            ServiceInterfacePtr service;
        };

        ServiceDesc m_services[SERVICE_PROVIDER_COUNT];
	};
}