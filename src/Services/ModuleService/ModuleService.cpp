#include "ModuleService.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ModuleFactoryInterface.h"
#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"

#include "Config/Algorithm.h"

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
#if defined(MENGINE_DEBUG)
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::hasModule( const ConstString & _name ) const
    {
        bool exist = VOCABULARY_HAS( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::runModule( const ConstString & _name, const DocumentInterfacePtr & _doc )
    {
        ModuleFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Module" ), _name );

        if( factory == nullptr )
        {
            LOGGER_ERROR( "invalid found module '%s'"
                , _name.c_str()
            );

            return false;
        }

        ModuleInterfacePtr module = factory->createModule( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( module );

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

        bool available = module->isAvailableModule();

        if( available == true )
        {
            m_modules.emplace_back( module );

            m_waits.erase( Algorithm::remove_if( m_waits.begin(), m_waits.end(), [_name]( const WaitModuleDesc & _desc )
            {
                return _desc.name == _name;
            } ), m_waits.end() );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::stopModule( const ConstString & _name )
    {
        ModuleInterfacePtr module = this->popModule_( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( module, "not found module '%s'"
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

        m_leaves.erase( Algorithm::remove_if( m_leaves.begin(), m_leaves.end(), [_name]( const LeaveModuleDesc & _desc )
        {
            return _desc.name == _name;
        } ), m_leaves.end() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::isRunModule( const ConstString & _name ) const
    {
        VectorModules::const_iterator it_found =
            Algorithm::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
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
    const ModuleInterfacePtr & ModuleService::getModule( const ConstString & _name ) const
    {
        const ModuleInterfacePtr & module = this->findModule_( _name );

        if( module == nullptr )
        {
            LOGGER_ERROR( "not found module '%s'"
                , _name.c_str()
            );

            return ModuleInterfacePtr::none();
        }

        return module;
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
    void ModuleService::beginUpdate( bool _focus )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            module->beginUpdate( _focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::endUpdate()
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            module->endUpdate();
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
    bool ModuleService::handleKeyEvent( const InputKeyEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleKeyEvent( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleTextEvent( const InputTextEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleTextEvent( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseButtonEvent( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseButtonEventBegin( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseButtonEventEnd( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseMove( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseWheel( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            if( module->handleMouseEnter( _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        for( const ModuleInterfacePtr & module : m_modules )
        {
            module->handleMouseLeave( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ModuleInterfacePtr & ModuleService::findModule_( const ConstString & _name ) const
    {
        VectorModules::const_iterator it_found =
            Algorithm::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
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
            Algorithm::find_if( m_modules.begin(), m_modules.end(), [&_name]( const ModuleInterfacePtr & _module )
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
    //////////////////////////////////////////////////////////////////////////
}