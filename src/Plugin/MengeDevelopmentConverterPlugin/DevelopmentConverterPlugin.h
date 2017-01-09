#	pragma once

#	include "Interface/ConverterInterface.h"

#	include "Core/PluginBase.h"

namespace Menge
{	
	class DevelopmentConverterPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "DevelopmentConverter" )

	public:
		DevelopmentConverterPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;
	};
}