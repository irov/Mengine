#	pragma once

#	include "Interface/PluginInterface.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class ServiceProvider
		: public ServiceProviderInterface
	{
	public:
		bool registryService( const String & _name, ServiceInterface * _service ) override;
		bool unregistryService( const String & _name ) override;

	public:
		ServiceInterface * getService( const String & _name ) override;

	public:
		bool addServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) override;
		bool removeServiceListener( const String & _name, ServiceListenerInterface * _serviceListener ) override;

	protected:
		typedef std::map<String, ServiceInterface *> TMapServices;
		TMapServices m_services;

		typedef std::vector<ServiceListenerInterface *> TVectorListenerServices;
		typedef std::map<String, TVectorListenerServices> TMapListenerServices;
		TMapListenerServices m_listeners;
	};
}