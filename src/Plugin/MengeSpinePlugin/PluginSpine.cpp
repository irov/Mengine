#	include "PluginSpine.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"
#	include "Kernel/ScriptClassWrapper.h"

#	include "Spine.h"
#	include "ResourceSpine.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MengeSpine, Menge::PluginSpine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PluginSpine::PluginSpine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginSpine::_initialize()
	{
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Spine" ), new NodePrototypeGenerator<Spine, 128> );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Resource" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceSpine" ), new ResourcePrototypeGenerator<ResourceSpine, 128> );

		pybind::interface_<Spine, pybind::bases<Node, Animatable> >( "Spine", false )
			.def( "setResourceSpine", &Spine::setResourceSpine )
			.def( "getResourceSpine", &Spine::getResourceSpine )
			.def( "setAnimationName", &Spine::setAnimationName )
			.def( "getAnimationName", &Spine::getAnimationName )
			.def( "setAnimationMix", &Spine::setAnimationMix )
			.def( "getDuration", &Spine::getDuration )
			;

		pybind::interface_<ResourceSpine, pybind::bases<ResourceReference> >( "ResourceSpine", false )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "Spine" ), new ClassScriptWrapper<Spine>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "ResourceSpine" ), new ClassScriptWrapper<ResourceSpine>() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginSpine::_finalize()
	{
	}
}