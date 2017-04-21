#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class BitmapFontPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "BitmapFont" )

	public:
		BitmapFontPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}