#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class Box2DPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "Box2D" )

	public:
		Box2DPlugin();
        ~Box2DPlugin() override;

	protected:
		bool _initializePlugin() override;
		void _finalizePlugin() override;
        void _destroyPlugin() override;
	};
}