#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

namespace Menge
{	
	class SoundCodecPlugin
		: public PluginInterface
	{
	public:
		SoundCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef std::vector<DecoderFactoryInterface *> TVectorDecoders;
        TVectorDecoders m_decoders;
	};
}