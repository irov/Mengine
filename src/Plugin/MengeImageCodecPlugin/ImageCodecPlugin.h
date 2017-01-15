#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"

#	include "Core/PluginBase.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class ImageCodecPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "ImageCodec" )

	public:
		ImageCodecPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		DataflowFactoryInterfacePtr m_factoryAEK;
		DataflowFactoryInterfacePtr m_factoryMDL;
	};
}