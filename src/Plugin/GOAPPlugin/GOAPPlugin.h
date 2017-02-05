#	pragma once

#	include "Core/PluginBase.h"
#	include "Core/ConstString.h"

#   include "PybindChain.h"
#   include "PybindSource.h"
#   include "PybindTask.h"
#   include "PybindTaskGenerator.h"

#   include "stdex/stl_map.h"

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

    protected:
        bool registryTask( const ConstString & _name, const pybind::object & _type );
        PybindTaskPtr generateTask( const ConstString & _name );

    protected:
        typedef stdex::map<ConstString, PybindTaskGeneratorPtr> TMapPybindTaskGenerator;
        TMapPybindTaskGenerator m_generators;
	};
}