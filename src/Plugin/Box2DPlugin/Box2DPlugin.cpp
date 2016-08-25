#	include "Box2DPlugin.h"

#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Box2DBody.h"
#	include "Box2DModule.h"

#	include "Core/ModuleFactory.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Box2D, Menge::Box2DPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Box2DPlugin::Box2DPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Box2DPlugin::_initialize()
	{
		MODULE_SERVICE( m_serviceProvider )
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBox2D" )
			, new ModuleFactory<Box2DModule>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBox2D" ) ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPlugin::_finalize()
	{
		MODULE_SERVICE( m_serviceProvider )
			->unregisterModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBox2D" ) );
	}
}
