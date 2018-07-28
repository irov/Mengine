#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
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
		typedef Vector<DecoderFactoryInterfacePtr> VectorDecoderFactories;
		VectorDecoderFactories m_decoderFactories;
	};
}

