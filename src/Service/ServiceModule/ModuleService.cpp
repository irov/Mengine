#include "ModuleService.h"

#include "Kernel/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ModuleService, Mengine::ModuleService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleService::ModuleService()        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleService::~ModuleService()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool ModuleService::_initializeService()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::_finalizeService()
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
			module->finalize();
		}

		m_modules.clear();
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
		VectorModules::iterator it_exist = 
			std::find_if( m_modules.begin(), m_modules.end(), FModuleFinder(_name) );

		if( it_exist != m_modules.end() )
		{
			return false;
		}
				
		MapModuleFactory::iterator it_found = m_moduleFactory.find( _name );

		if( it_found == m_moduleFactory.end() )
		{
			return false;
		}
		
		const ModuleFactoryInterfacePtr & factory = it_found->second;

		ModuleInterfacePtr module = factory->createModule( _name );

		if( module->avaliable() == false )
		{
			return true;
		}
		
		if( module->initialize() == false )
		{
			return false;
		}

		m_modules.emplace_back( module );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::stopModule( const ConstString & _name )
	{
		VectorModules::iterator it_found = 
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
	void ModuleService::update( bool _focus )
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
			module->update( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::tick( const UpdateContext * _context )
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
            module->tick( _context );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::render( const RenderContext * _state )
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
			module->render( _state );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::message( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params )
	{
		const ModuleInterfacePtr & module = this->findModule( _moduleName );

		if( module == nullptr )
		{
			return;
		}

		module->message( _messageName, _params );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::messageAll( const ConstString & _messageName, const MapWParams & _params )
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
			module->messageAll( _messageName, _params );
		}
	}
	//////////////////////////////////////////////////////////////////////////		
	const ModuleInterfacePtr & ModuleService::findModule( const ConstString & _moduleName ) const
	{
        for( const ModuleInterfacePtr & module : m_modules )
		{
			if( module->getName() != _moduleName )
			{
				continue;
			}

			return module;
		}

		return ModuleInterfacePtr::none();
	}
}