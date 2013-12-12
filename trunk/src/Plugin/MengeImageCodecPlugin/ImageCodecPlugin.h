#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"

#	include <vector>

namespace Menge
{
	class ImageCodecPlugin
		: public PluginInterface
	{
	public:
		ImageCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void destroy() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<DecoderFactoryInterfacePtr> TVectorDecoders;
		TVectorDecoders m_decoders;

		typedef std::vector<EncoderFactoryInterfacePtr> TVectorEncoders;
		TVectorEncoders m_encoders;

		DataflowFactoryInterfacePtr m_factoryAEK;
	};
}