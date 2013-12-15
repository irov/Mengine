#	include "PluginPathFinder.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ModuleInterface.h"

#	include "Factory/FactoryDefault.h"

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
	class PathFinderFactoryInterface
		: public ModuleFactoryInterface
	{
	public:
		PathFinderFactoryInterface( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
		{
		}

	public:
		ModuleInterfacePtr createModule() override
		{
			ModulePathFinder * modulePathFinder = m_factory.createObjectT();
			modulePathFinder->setServiceProvider( m_serviceProvider );

			return modulePathFinder;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef FactoryDefault<ModulePathFinder> TFactoryModulePathFinder;
		TFactoryModulePathFinder m_factory;
	};
	//////////////////////////////////////////////////////////////////////////
	PluginPathFinder::PluginPathFinder()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PluginPathFinder::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_factoryModulePathFinder = new PathFinderFactoryInterface(m_serviceProvider);
		
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder"), m_factoryModulePathFinder );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::destroy()
	{
		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder") );

		m_factoryModulePathFinder = nullptr;

		delete this;
	}
}