#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ModuleInterface.h"

namespace Menge
{	
	class PluginPathFinder
		: public PluginInterface
	{
	public:
		PluginPathFinder();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ModuleFactoryInterfacePtr m_factoryModulePathFinder;
		ModuleFactoryInterfacePtr m_factoryModuleAreaOfInterest;
		ModuleFactoryInterfacePtr m_factoryModuleCollisionGround;
		ModuleFactoryInterfacePtr m_factoryModuleBurritoWorld;
	};
}