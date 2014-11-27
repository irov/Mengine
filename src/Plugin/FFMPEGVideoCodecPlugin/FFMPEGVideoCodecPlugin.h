#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class FFMPEGVideoCodecPlugin
		: public PluginInterface
	{
	public:
		FFMPEGVideoCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void finalize() override;

	protected:
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::vector<DecoderFactoryInterfacePtr> TVectorVideoDecoders;
		TVectorVideoDecoders m_decoders;
	};
}

