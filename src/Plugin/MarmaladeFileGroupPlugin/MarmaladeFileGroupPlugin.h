#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class MarmaladeFileGroupPlugin
		: public PluginBase
	{
	public:
		MarmaladeFileGroupPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}