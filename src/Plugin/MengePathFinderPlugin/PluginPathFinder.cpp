#	include "PluginPathFinder.h"

#	include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"
#	include "Core/ModuleFactory.h"

#	include "ModulePathFinder.h"
#	include "ModuleAreaOfInterest.h"
#	include "ModuleCollision.h"
#	include "ModuleInvader.h"
#	include "ModuleBurritoWorld.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PathFinder, Menge::PluginPathFinder )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	PluginPathFinder::PluginPathFinder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginPathFinder::_initialize()
	{
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModulePathFinder" )
			, new ModuleFactory<ModulePathFinder>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModulePathFinder" ) ) );

		MODULE_SERVICE(m_serviceProvider)
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleAreaOfInterest" )
			, new ModuleFactory<ModuleAreaOfInterest>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleAreaOfInterest" ) ) );
		
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleCollision" )
			, new ModuleFactory<ModuleCollision>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleCollision" ) ) );

		MODULE_SERVICE( m_serviceProvider )
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleInvader" )
			, new ModuleFactory<ModuleInvader>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleInvader" ) ) );		

		MODULE_SERVICE(m_serviceProvider)
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBurritoWorld" )
			, new ModuleFactory<ModuleBurritoWorld>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBurritoWorld" ) ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::_finalize()
	{
		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModulePathFinder") );

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleAreaOfInterest") );

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleCollision") );

		MODULE_SERVICE( m_serviceProvider )
			->unregisterModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleInvader" ) );		

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ModuleBurritoWorld") );
	}
}