#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Box2DPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Box2D" )

	public:
		Box2DPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}