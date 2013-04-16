#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ConverterInterface.h"



namespace Menge
{	
	class SoundConverterPlugin
		: public PluginInterface
	{
	public:
		SoundConverterPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<ConverterFactoryInterface *> TVectorSoundConverters;
		TVectorSoundConverters m_converters;
	};
}