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
        ModuleFactoryInterfacePtr moduleStreamFactory = new ModuleFactory<ModuleSteam>();
        
		MODULE_SERVICE()
			->registerModule( STRINGIZE_STRING_LOCAL( "ModuleSteam" ), moduleStreamFactory);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SteamPlugin::_finalize()
	{
		MODULE_SERVICE()
			->unregisterModule( STRINGIZE_STRING_LOCAL( "ModuleSteam" ) );
	}
}
