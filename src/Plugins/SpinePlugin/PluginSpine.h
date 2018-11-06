#	pragma once

#	include "Core/PluginBase.h"

namespace Mengine
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