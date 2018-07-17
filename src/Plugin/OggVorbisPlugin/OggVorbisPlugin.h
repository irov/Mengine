#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{	
	class OggVorbisPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "OggVorbis" )

	public:
		OggVorbisPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;		
	};
}