#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class PosixFileGroupPlugin
		: public PluginBase
	{
        PLUGIN_DECLARE("PosixFileGroup")

	public:
		PosixFileGroupPlugin();

	protected:
        bool _initialize() override;
        void _finalize() override;
	};
}