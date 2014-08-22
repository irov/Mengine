#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"

#	include <stdex/stl_vector.h>

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

		typedef stdex::vector<DecoderFactoryInterfacePtr> TVectorDecoders;
		TVectorDecoders m_decoders;

		typedef stdex::vector<EncoderFactoryInterfacePtr> TVectorEncoders;
		TVectorEncoders m_encoders;

		DataflowFactoryInterfacePtr m_factoryAEK;
		DataflowFactoryInterfacePtr m_factoryMDL;
	};
}