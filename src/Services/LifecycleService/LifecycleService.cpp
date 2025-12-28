#include "LifecycleService.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LifecycleService, Mengine::LifecycleService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LifecycleService::LifecycleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LifecycleService::~LifecycleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerService( ServiceInterface * _service )
    {
        LifecycleInterface * dispatchable = _service->getLifecycleable();

        if( dispatchable == nullptr )
        {
            return;
        }

        this->registerLifecycle( dispatchable );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterService( ServiceInterface * _service )
    {
        LifecycleInterface * dispatchable = _service->getLifecycleable();
        
        if( dispatchable == nullptr )
        {
            return;
        }

        this->unregisterLifecycle( dispatchable );
    }
    //////////////////////////////////////////////////////////////////////////
    bool LifecycleService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::_finalizeService()
    {
        m_dispatchable.erase( StdAlgorithm::remove_if( m_dispatchable.begin(), m_dispatchable.end(), []( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == nullptr;
        } ), m_dispatchable.end() );

        m_dispatchableAdd.erase( StdAlgorithm::remove_if( m_dispatchableAdd.begin(), m_dispatchableAdd.end(), []( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == nullptr;
        } ), m_dispatchableAdd.end() );

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const DispatchableDesc & desc : m_dispatchable )
        {
            LOGGER_ASSERTION( "was forgotten remove dispatchable '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.dispatchable )
            );
        }

        for( const DispatchableDesc & desc : m_dispatchableAdd )
        {
            LOGGER_ASSERTION( "was forgotten remove dispatchable '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.dispatchable )
            );
        }
#endif

        m_dispatchable.clear();
        m_dispatchableAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerLifecycle( LifecycleInterface * _dispatchable )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dispatchable, "dispatchable is nullptr" );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_dispatchable.begin(), m_dispatchable.end(), [_dispatchable]( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == _dispatchable;
        } ) == m_dispatchable.end(), "dispatchable '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _dispatchable )
        );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_dispatchableAdd.begin(), m_dispatchableAdd.end(), [_dispatchable]( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == _dispatchable;
        } ) == m_dispatchableAdd.end(), "dispatchable '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _dispatchable )
        );

        DispatchableDesc desc;
        desc.dispatchable = _dispatchable;

        m_dispatchableAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterLifecycle( LifecycleInterface * _dispatchable )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dispatchable, "dispatchable is nullptr" );

        VectorDispatchable::iterator it_found = StdAlgorithm::find_if( m_dispatchable.begin(), m_dispatchable.end(), [_dispatchable]( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == _dispatchable;
        } );

        if( it_found != m_dispatchable.end() )
        {
            m_dispatchable.erase( it_found );

            return;
        }

        VectorDispatchable::iterator it_add_found = StdAlgorithm::find_if( m_dispatchableAdd.begin(), m_dispatchableAdd.end(), [_dispatchable]( const DispatchableDesc & _desc )
        {
            return _desc.dispatchable == _dispatchable;
        } );

        if( it_add_found != m_dispatchableAdd.end() )
        {
            m_dispatchableAdd.erase( it_add_found );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::preUpdate()
    {
        if( m_dispatchableAdd.empty() == false )
        {
            m_dispatchable.insert( m_dispatchable.end(), m_dispatchableAdd.begin(), m_dispatchableAdd.end() );
            m_dispatchableAdd.clear();
        }

        for( const DispatchableDesc & desc : m_dispatchable )
        {
            desc.dispatchable->preUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::update()
    {
        for( const DispatchableDesc & desc : m_dispatchable )
        {
            desc.dispatchable->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::postUpdate()
    {
        for( const DispatchableDesc & desc : m_dispatchable )
        {
            desc.dispatchable->postUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

