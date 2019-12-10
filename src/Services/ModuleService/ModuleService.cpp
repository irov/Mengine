#include "ModuleService.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ModuleFactoryInterface.h"
#include "Interface/TimepipeServiceInterface.h"

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
        : m_timepipe( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleService::~ModuleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::_initializeService()
    {
        uint32_t timepipe = TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_timepipe = timepipe;

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

        m_waits.clear();
        m_leaves.clear();

        TIMEPIPE_SERVICE()
            ->removeTimepipe( m_timepipe );

        m_timepipe = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::hasModule( const ConstString & _name ) const
    {
        bool exist = VOCABULARY_HAS( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::runModule( const ConstString & _name, const DocumentPtr & _doc )
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

        for( const WaitModuleDesc & desc : m_waits )
        {
            if( desc.name != _name )
            {
                continue;
            }

            desc.lambda();

            break;
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

        for( const LeaveModuleDesc & desc : m_leaves )
        {
            if( desc.name != _name )
            {
                continue;
            }

            desc.lambda();

            break;
        }

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
    void ModuleService::waitModule( const ConstString & _name, const LambdaWaitModule & _lambda )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            const ConstString & name = module->getName();

            if( name != _name )
            {
                continue;
            }

            if( module->isInitializeModule() == false )
            {
                break;
            }

            _lambda();

            return;
        }

        WaitModuleDesc desc;
        desc.name = _name;
        desc.lambda = _lambda;

        m_waits.push_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::leaveModule( const ConstString & _name, const LambdaLeaveModule & _lambda )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            const ConstString & name = module->getName();

            if( name != _name )
            {
                continue;
            }

            if( module->isInitializeModule() == true )
            {
                break;
            }

            _lambda();

            return;
        }

        LeaveModuleDesc desc;
        desc.name = _name;
        desc.lambda = _lambda;

        m_leaves.push_back( desc );
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
    void ModuleService::onTimepipe( const UpdateContext * _context )
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