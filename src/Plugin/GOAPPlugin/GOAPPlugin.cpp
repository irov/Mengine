#	include "SteamPlugin.h"

#	include "Interface/StringizeInterface.h"

#	include "ModuleSteam.h"

#	include "Core/ModuleFactory.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Steam, Menge::SteamPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SteamPlugin::SteamPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SteamPlugin::_initialize()
	{
		MODULE_SERVICE( m_serviceProvider )
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleSteam" )
			, new ModuleFactory<ModuleSteam>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleSteam" ) ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SteamPlugin::_finalize()
	{
		MODULE_SERVICE( m_serviceProvider )
			->unregisterModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleSteam" ) );
	}
}
