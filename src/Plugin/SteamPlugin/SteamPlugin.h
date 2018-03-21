#pragma once

#include "Core/PluginBase.h"

#include "steam/steam_api.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class SteamPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Steam" )

	public:
		SteamPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}