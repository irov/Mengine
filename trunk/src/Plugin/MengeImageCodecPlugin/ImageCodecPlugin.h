#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"

#	include <vector>

namespace Menge
{
	class ImageCodecPlugin
		: public PluginInterface
	{
	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<DecoderFactoryInterface *> TVectorDecoders;
		TVectorDecoders m_decoders;

		typedef std::vector<EncoderFactoryInterface *> TVectorEncoders;
		TVectorEncoders m_encoders;
	};
}