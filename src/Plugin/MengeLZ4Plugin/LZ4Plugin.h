#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class LZ4Plugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "LZ4" );

	public:
		LZ4Plugin();

	protected:
		bool _initialize() override;
		void _finalize() override;		
	};
}