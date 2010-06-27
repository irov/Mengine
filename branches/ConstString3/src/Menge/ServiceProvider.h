#	pragma once

#	include "Interface/PluginInterface.h"

#	include <map>

namespace Menge
{
	class ServiceProvider
		: ServiceProviderInterface
	{
	public:
		void registryService( const String & _name, ServiceInterface * _service );

	public:
		ServiceInterface * getService( const String & _name ) override;

	protected:
		typedef std::map<String, ServiceInterface> TMapServices;
		TMapServices m_services;
	};
}