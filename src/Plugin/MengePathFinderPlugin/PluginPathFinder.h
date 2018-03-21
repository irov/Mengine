#pragma once

#include "Core/PluginBase.h"

namespace Mengine
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