#include "SteamPlugin.h"

#include "Interface/StringizeInterface.h"

#include "ModuleSteam.h"

#include "Kernel/ModuleFactory.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Steam, Mengine::SteamPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	SteamPlugin::SteamPlugin()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    SteamPlugin::~SteamPlugin()
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
