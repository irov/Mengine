#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ConverterInterface.h"

namespace Menge
{	
	class HotspotImageConverterPlugin
		: public PluginInterface
	{
	public:
		HotspotImageConverterPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<ConverterFactoryInterface *> TVectorHotspotImageConverters;
		TVectorHotspotImageConverters m_converters;
	};
}