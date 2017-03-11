#	include "PluginPathFinder.h"

#	include "Interface/StringizeInterface.h"

#	include "Core/ModuleFactory.h"

#	include "ModulePathFinder.h"
#	include "ModuleAreaOfInterest.h"
#	include "ModuleCollision.h"
#	include "ModuleMagnet.h"
#	include "ModuleInvader.h"
#	include "ModuleBurritoWorld.h"
#	include "ModuleBounce.h"

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
		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModulePathFinder" )
			, new ModuleFactory<ModulePathFinder>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleAreaOfInterest" )
			, new ModuleFactory<ModuleAreaOfInterest>() );
		
		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleCollision" )
			, new ModuleFactory<ModuleCollision>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleMagnet" )
			, new ModuleFactory<ModuleMagnet>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleInvader" )
			, new ModuleFactory<ModuleInvader>() );		

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBurritoWorld" )
			, new ModuleFactory<ModuleBurritoWorld>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ModuleBounce" )
			, new ModuleFactory<ModuleBounce>() );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::_finalize()
	{
	}
}