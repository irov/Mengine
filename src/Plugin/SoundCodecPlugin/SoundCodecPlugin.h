#pragma once

#include "Interface/CodecInterface.h"

#include "Core/PluginBase.h"

namespace Mengine
{	
	class SoundCodecPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "SoundCodec" )

	public:
		SoundCodecPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}