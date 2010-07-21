#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class XmlCodecPlugin
		: public PluginInterface
	{
	public:
		void initialize( ServiceProviderInterface * _provider ) override;
	};
}