#	include "PluginPathFinder.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ModuleInterface.h"

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginPathFinder( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::PluginPathFinder();

		return true;
	}

#   ifdef MENGINE_PLUGIN_DLL_PATH_FINDER
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

		m_modulePathFinder = new ModulePathFinder;
		m_modulePathFinder->setServiceProvider( m_serviceProvider );
		
		//MODULE_SERVICE(m_serviceProvider)
			//->addModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder"), m_modulePathFinder );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::destroy()
	{
		//MODULE_SERVICE(m_serviceProvider)
			//->removeModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder") );

		delete this;
	}
}