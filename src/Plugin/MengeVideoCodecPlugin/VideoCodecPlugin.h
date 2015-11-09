#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/PluginBase.h"

namespace Menge
{
	class VideoCodecPlugin
		: public PluginBase
	{
	public:
		PLUGIN_DECLARE( "VideoCodec" )

	public:
		VideoCodecPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		typedef stdex::vector<DecoderFactoryInterfacePtr> TVectorVideoDecoders;
		TVectorVideoDecoders m_decoders;
	};
}

