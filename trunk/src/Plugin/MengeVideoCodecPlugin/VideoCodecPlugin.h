#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{
	class VideoCodecPlugin
		: public PluginInterface
	{
	public:
		VideoCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<DecoderFactoryInterface *> TVectorVideoDecoders;
		TVectorVideoDecoders m_decoders;
	};
}

