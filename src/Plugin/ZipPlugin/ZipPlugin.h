#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
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