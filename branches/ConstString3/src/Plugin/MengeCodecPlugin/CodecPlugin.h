#	pragma once

#	include "Interface/PluginInterface.h"

namespace Menge
{
	class CodecPlugin
		: public PluginInterface
	{
	public:
		void initialize( ServiceProviderInterface * _provider ) override;
	};
}