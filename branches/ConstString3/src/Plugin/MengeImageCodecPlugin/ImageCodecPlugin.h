#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class ImageCodecPlugin
		: public PluginInterface
	{
	public:
		void initialize( ServiceProviderInterface * _provider ) override;
	};
}