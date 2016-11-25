#	pragma once

#	include "Interface/AmplifierInterface.h"

#	include "Core/PluginBase.h"

namespace Menge
{
	class MarmaladeGoogleAdMobPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "MarmaladeGoogleAdMob" )

	public:
		MarmaladeGoogleAdMobPlugin();

	public:
		bool _initialize() override;
		void _finalize() override;
	};
}