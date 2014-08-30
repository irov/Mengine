#	include "PluginPathFinder.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"
#	include "Kernel/ModuleFactory.h"

#	include "ModulePathFinder.h"
#	include "ModuleAreaOfInterest.h"
#	include "ModuleCollisionGround.h"


extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginPathFinder( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::PluginPathFinder();

		return true;
	}

#   ifdef MENGE_PLUGIN_DLL
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginPathFinder( _plugin );
	}
#   endif
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	PluginPathFinder::PluginPathFinder()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginPathFinder::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_factoryModulePathFinder = new ModuleFactory<ModulePathFinder>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ModulePathFinder"));
		m_factoryModuleAreaOfInterest = new ModuleFactory<ModuleAreaOfInterest>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ModuleAreaOfInterest"));
		m_factoryModuleCollisionGround = new ModuleFactory<ModuleCollisionGround>(m_serviceProvider, CONST_STRING_LOCAL(m_serviceProvider, "ModuleCollisionGround"));
		
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( CONST_STRING_LOCAL(m_serviceProvider, "ModulePathFinder"), m_factoryModulePathFinder );

		MODULE_SERVICE(m_serviceProvider)
			->registerModule( CONST_STRING_LOCAL(m_serviceProvider, "ModuleAreaOfInterest"), m_factoryModuleAreaOfInterest );
		
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( CONST_STRING_LOCAL(m_serviceProvider, "ModuleCollisionGround"), m_factoryModuleCollisionGround );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::finalize()
	{
		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( CONST_STRING_LOCAL(m_serviceProvider, "ModulePathFinder") );

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( CONST_STRING_LOCAL(m_serviceProvider, "ModuleAreaOfInterest") );

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( CONST_STRING_LOCAL(m_serviceProvider, "ModuleCollisionGround") );

		m_factoryModulePathFinder = nullptr;
		m_factoryModuleAreaOfInterest = nullptr;
		m_factoryModuleCollisionGround = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::destroy()
	{
		delete this;
	}
}