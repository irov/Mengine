#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class PluginPathFinder
		: public PluginBase
	{
		PLUGIN_DECLARE( "PathFinder" )

	public:
		PluginPathFinder();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}