#	pragma once

#	include "Interface/CodecInterface.h"

#	include "Core/PluginBase.h"

namespace Menge
{
	class XmlCodecPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "XmlCodec" )

	public:
		XmlCodecPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}