#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class ZipPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Zip" )

	public:
		ZipPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}