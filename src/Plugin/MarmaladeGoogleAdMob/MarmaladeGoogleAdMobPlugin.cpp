#	include "MarmaladeGoogleAdMobPlugin.h"

#	include "Interface/StringizeInterface.h"

#	include "Core/ModuleFactory.h"

#	include "MarmaladeGoogleAdMobModule.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MarmaladeGoogleAdMob, Menge::MarmaladeGoogleAdMobPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeGoogleAdMobPlugin::MarmaladeGoogleAdMobPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeGoogleAdMobPlugin::_initialize()
	{			
		MODULE_SERVICE( m_serviceProvider )
			->registerModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleGoogleAdMob" )
			, new ModuleFactory<MarmaladeGoogleAdMobModule>( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleGoogleAdMob" ) ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeGoogleAdMobPlugin::_finalize()
	{	
		MODULE_SERVICE( m_serviceProvider )
			->unregisterModule( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleGoogleAdMob" ) );

	}
}