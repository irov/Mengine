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
            module->finalizeModule();
        }

        m_modules.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::registerModule( const ConstString & _moduleName, const ModuleFactoryInterfacePtr & _module )
    {
        m_moduleFactory.insert( _moduleName, _module );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::unregisterModule( const ConstString & _moduleName )
    {
        m_moduleFactory.erase( _moduleName );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::runModule( const ConstString & _moduleName, const Char * _doc )
    {
        if( this->findModule( _moduleName ) != nullptr )
        {
            return false;
        }

        const ModuleFactoryInterfacePtr & factory = m_moduleFactory.find( _moduleName );

        if( factory == nullptr )
        {
            return false;
        }

        ModuleInterfacePtr module = factory->createModule( _moduleName, _doc );

        if( module->initializeModule() == false )
        {
            return false;
        }

        m_modules.emplace_back( module );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::stopModule( const ConstString & _moduleName )
    {
        ModuleInterfacePtr module = this->popModule( _moduleName );

        if( module == nullptr )
        {
            return false;
        }

        module->finalizeModule();

        return true;
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
        VectorModules::const_iterator it_found =
            std::find_if( m_modules.begin(), m_modules.end(), [_moduleName]( const ModuleInterfacePtr & _module )
        {
            return _module->getName() == _moduleName;
        } );

        if( it_found == m_modules.end() )
        {
            return ModuleInterfacePtr::none();
        }

        const ModuleInterfacePtr & module = *it_found;

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleInterfacePtr ModuleService::popModule( const ConstString & _moduleName )
    {
        VectorModules::iterator it_found =
            std::find_if( m_modules.begin(), m_modules.end(), [_moduleName]( const ModuleInterfacePtr & _module )
        {
            return _module->getName() == _moduleName;
        } );

        if( it_found == m_modules.end() )
        {
            return nullptr;
        }

        ModuleInterfacePtr module = *it_found;

        m_modules.erase( it_found );

        return module;
    }
}