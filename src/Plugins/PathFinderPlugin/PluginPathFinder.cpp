#include "PluginPathFinder.h"

#include "Interface/StringizeInterface.h"

#include "Core/ModuleFactory.h"

#include "ModulePathFinder.h"
#include "ModuleAreaOfInterest.h"
#include "ModuleCollision.h"
#include "ModuleMagnet.h"
#include "ModuleInvader.h"
#include "ModuleBurritoWorld.h"
#include "ModuleBounce.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PathFinder, Mengine::PluginPathFinder )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{	
	//////////////////////////////////////////////////////////////////////////
	PluginPathFinder::PluginPathFinder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginPathFinder::_initialize()
	{		
		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModulePathFinder" )
			, new ModuleFactory<ModulePathFinder>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAreaOfInterest" )
			, new ModuleFactory<ModuleAreaOfInterest>() );
		
		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleCollision" )
			, new ModuleFactory<ModuleCollision>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleMagnet" )
			, new ModuleFactory<ModuleMagnet>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleInvader" )
			, new ModuleFactory<ModuleInvader>() );		

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleBurritoWorld" )
			, new ModuleFactory<ModuleBurritoWorld>() );

		this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleBounce" )
			, new ModuleFactory<ModuleBounce>() );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::_finalize()
	{
	}
}