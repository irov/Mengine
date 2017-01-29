#	pragma once

#	include "Core/PluginBase.h"

#   include "PybindChain.h"
#   include "PybindSource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class GOAPPlugin
		: public PluginBase
	{
		PLUGIN_DECLARE( "GOAP" )

	public:
		GOAPPlugin();

	protected:
		bool _initialize() override;
		void _finalize() override;

    protected:
        PybindChainPtr makeChain( const PybindSourcePtr & _source );
	};
}