#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ConverterInterface.h"



namespace Menge
{	
	class VideoConverterPlugin
		: public PluginInterface
	{
	public:
		VideoConverterPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<ConverterFactoryInterface *> TVectorVideoConverters;
		TVectorVideoConverters m_converters;
	};
}