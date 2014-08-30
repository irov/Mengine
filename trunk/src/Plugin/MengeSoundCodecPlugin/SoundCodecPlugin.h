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
		void finalize() override;

	protected:
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef stdex::vector<DecoderFactoryInterfacePtr> TVectorDecoders;
        TVectorDecoders m_decoders;
	};
}