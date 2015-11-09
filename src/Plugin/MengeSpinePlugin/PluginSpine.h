#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class PluginSpine
		: public PluginBase
	{
		PLUGIN_DECLARE( "Spine" )

	public:
		PluginSpine();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}