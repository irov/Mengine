#	pragma once

#	include "Kernel/PluginBase.h"

namespace Mengine
{
	class SpinePlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Spine" )

	public:
		SpinePlugin();
        ~SpinePlugin() override;

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}