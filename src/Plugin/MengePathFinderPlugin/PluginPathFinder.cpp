#	include "PluginPathFinder.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"

#	include "ModulePathFinder.h"
#	include "ModuleCollisionGround.h"


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
	template<class T>
	class ModuleFactory
		: public ModuleFactoryInterface
	{
	public:
		ModuleFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
			: m_serviceProvider(_serviceProvider)
			, m_name(_name)
		{
		}

	public:
		ModuleInterfacePtr createModule() override
		{
			ModuleInterface * module = m_factory.createObjectT();
			module->setServiceProvider( m_serviceProvider );
			module->setName( m_name );

			return module;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef FactoryDefaultStore<T> TFactoryModule;
		TFactoryModule m_factory;
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

		m_factoryModulePathFinder = new ModuleFactory<ModulePathFinder>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ModulePathFinder"));
		m_factoryModuleCollisionGround = new ModuleFactory<ModuleCollisionGround>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ModuleCollisionGround"));
		
		MODULE_SERVICE(m_serviceProvider)
			->registerModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder"), m_factoryModulePathFinder );

		MODULE_SERVICE(m_serviceProvider)
			->registerModule( Helper::stringizeString(m_serviceProvider, "ModuleCollisionGround"), m_factoryModuleCollisionGround );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PluginPathFinder::destroy()
	{
		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( Helper::stringizeString(m_serviceProvider, "ModulePathFinder") );

		MODULE_SERVICE(m_serviceProvider)
			->unregisterModule( Helper::stringizeString(m_serviceProvider, "ModuleCollisionGround") );

		m_factoryModulePathFinder = nullptr;
		m_factoryModuleCollisionGround = nullptr;

		delete this;
	}
}