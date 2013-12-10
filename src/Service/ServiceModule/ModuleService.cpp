#   include "ModuleService.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ModuleService, Menge::ModuleServiceInterface, Menge::ModuleService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ModuleService::ModuleService()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleService::~ModuleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ModuleService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ModuleService::registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _module )
	{
		m_moduleFactory.insert( std::make_pair(_name, _module) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::unregisterModule( const ConstString & _name )
	{
		m_moduleFactory.erase( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleService::runModule( const ConstString & _name )
	{
		TMapModules::iterator it_module_found = m_modules.find( _name );

		if( it_module_found != m_modules.end() )
		{
			return false;
		}

		TMapModuleFactory::iterator it_found = m_moduleFactory.find( _name );

		if( it_found == m_moduleFactory.end() )
		{
			return false;
		}
		
		const ModuleFactoryInterfacePtr & factory = it_found->second;

		ModuleInterfacePtr module = factory->createModule();

		module->setServiceProvider( m_serviceProvider );

		if( module->initialize() == false )
		{
			return false;
		}

		m_modules[_name] = module;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::stopModule( const ConstString & _name )
	{
		TMapModules::iterator it_found = m_modules.find( _name );

		if( it_found == m_modules.end() )
		{
			return;
		}

		const ModuleInterfacePtr & module = it_found->second;

		module->finalize();
		
		m_modules.erase( it_found );
	}
}