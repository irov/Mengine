#include "ModuleService.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"

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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::_finalizeService()
    {        
#ifdef MENGINE_DEBUG
        for( const ModuleInterfacePtr & module : m_modules )
        {
            LOGGER_ERROR( "was forgotten stop module '%s'"
                , module->getName().c_str()
            );
        }
#endif

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Module" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::hasModule( const ConstString & _name ) const
    {
        bool exist = VOCABULARY_HAS( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::runModule( const ConstString & _name, const Char * _doc )
    {
        ModuleFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, false );

        ModuleInterfacePtr module = factory->createModule( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( module, false );

        module->setName( _name );

        if( module->initializeModule() == false )
        {
            LOGGER_ERROR( "invalid initialize module '%s'"
                , _name.c_str()
            );

            return false;
        }

        m_modules.emplace_back( module );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::stopModule( const ConstString & _name )
    {
        ModuleInterfacePtr module = this->popModule_( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( module, false, "not found module '%s'"
            , _name.c_str()
        );

        module->finalizeModule();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::isRunModule( const ConstString & _name ) const
    {
        VectorModules::const_iterator it_found =
            std::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
        {
            return _module->getName() == _name;
        } );

        if( it_found == m_modules.end() )
        {
            return false;
        }

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
    void ModuleService::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            module->render( _renderPipeline, _context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::message( const ConstString & _name, const ConstString & _messageName, const MapWParams & _params )
    {
        const ModuleInterfacePtr & module = this->findModule_( _name );

        if( module == nullptr )
        {
            return;
        }

        module->message( _messageName, _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::messageAll( const ConstString & _name, const MapWParams & _params )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            module->messageAll( _name, _params );
        }
    }
    //////////////////////////////////////////////////////////////////////////		
    const ModuleInterfacePtr & ModuleService::findModule_( const ConstString & _name ) const
    {
        VectorModules::const_iterator it_found =
            std::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
        {
            return _module->getName() == _name;
        } );

        if( it_found == m_modules.end() )
        {
            return ModuleInterfacePtr::none();
        }

        const ModuleInterfacePtr & module = *it_found;

        return module;
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleInterfacePtr ModuleService::popModule_( const ConstString & _name )
    {
        VectorModules::iterator it_found =
            std::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
        {
            return _module->getName() == _name;
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