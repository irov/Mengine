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
	namespace
	{
		class FModuleFinder
		{
		public:
			FModuleFinder( const ConstString & _name )
				: m_name(_name)
			{			
			}

		public:
			bool operator() ( const ModuleInterfacePtr & _module )
			{
				const ConstString & moduleName = _module->getName();

				return m_name == moduleName;
			}

		protected:
			const ConstString & m_name;

		private:
			void operator = ( const FModuleFinder & )
			{
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleService::runModule( const ConstString & _name )
	{
		TVectorModules::iterator it_exist = 
			std::find_if( m_modules.begin(), m_modules.end(), FModuleFinder(_name) );

		if( it_exist != m_modules.end() )
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
		
		if( module->initialize() == false )
		{
			return false;
		}

		m_modules.push_back( module );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::stopModule( const ConstString & _name )
	{
		TVectorModules::iterator it_found = 
			std::find_if( m_modules.begin(), m_modules.end(), FModuleFinder(_name) );

		if( it_found == m_modules.end() )
		{
			return;
		}

		const ModuleInterfacePtr & module = *it_found;

		module->finalize();
		
		m_modules.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::update( float _time, float _timing )
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->update( _time, _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, uint32_t _debugMask )
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->render( _viewport, _camera, _clipplane, _debugMask );
		}
	}
}