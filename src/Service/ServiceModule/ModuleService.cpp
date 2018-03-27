#include "ModuleService.h"

#include "Logger/Logger.h"

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
	bool ModuleService::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::_finalize()
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
			it != it_end;
			++it )
		{
			const ModuleInterfacePtr & module = *it;

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

		ModuleInterfacePtr module = factory->createModule( _name );

		if( module->avaliable() == false )
		{
			return true;
		}
		
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
	void ModuleService::update( bool _focus )
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->update( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::tick( float _time, float _timing )
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->tick( _time, _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::render( const RenderState * _state, uint32_t _debugMask )
	{
		for( TVectorModules::iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->render( _state, _debugMask );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::message( const ConstString & _moduleName, const ConstString & _messageName, const TMapWParams & _params )
	{
		const ModuleInterfacePtr & module = this->findModule( _moduleName );

		if( module == nullptr )
		{
			return;
		}

		module->message( _messageName, _params );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleService::messageAll( const ConstString & _messageName, const TMapWParams & _params )
	{
		for( TVectorModules::const_iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			module->messageAll( _messageName, _params );
		}
	}
	//////////////////////////////////////////////////////////////////////////		
	const ModuleInterfacePtr & ModuleService::findModule( const ConstString & _moduleName ) const
	{
		for( TVectorModules::const_iterator
			it = m_modules.begin(),
			it_end = m_modules.end();
		it != it_end;
		++it )
		{
			const ModuleInterfacePtr & module = *it;

			if( module->getName() != _moduleName )
			{
				continue;
			}

			return module;
		}

		return ModuleInterfacePtr::none();
	}
}