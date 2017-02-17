#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class TTFPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "TTF" )

	public:
		TTFPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}