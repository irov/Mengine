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
        LifecycleInterface * lifecycle = _service->getLifecycleable();

        if( lifecycle == nullptr )
        {
            return;
        }

        this->registerLifecycle( lifecycle );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterService( ServiceInterface * _service )
    {
        LifecycleInterface * lifecycle = _service->getLifecycleable();
        
        if( lifecycle == nullptr )
        {
            return;
        }

        this->unregisterLifecycle( lifecycle );
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
        m_lifecycles.erase( StdAlgorithm::remove_if( m_lifecycles.begin(), m_lifecycles.end(), []( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == nullptr;
        } ), m_lifecycles.end() );

        m_lifecyclesAdd.erase( StdAlgorithm::remove_if( m_lifecyclesAdd.begin(), m_lifecyclesAdd.end(), []( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == nullptr;
        } ), m_lifecyclesAdd.end() );

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            LOGGER_ASSERTION( "was forgotten remove lifecycle '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.lifecycle )
            );
        }

        for( const LifecycleDesc & desc : m_lifecyclesAdd )
        {
            LOGGER_ASSERTION( "was forgotten remove lifecycle '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.lifecycle )
            );
        }
#endif

        m_lifecycles.clear();
        m_lifecyclesAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerLifecycle( LifecycleInterface * _lifecycle )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _lifecycle, "lifecycle is nullptr" );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_lifecycles.begin(), m_lifecycles.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } ) == m_lifecycles.end(), "lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _lifecycle )
        );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_lifecyclesAdd.begin(), m_lifecyclesAdd.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } ) == m_lifecyclesAdd.end(), "lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _lifecycle )
        );

        LifecycleDesc desc;
        desc.lifecycle = _lifecycle;

        m_lifecyclesAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterLifecycle( LifecycleInterface * _lifecycle )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _lifecycle, "lifecycle is nullptr" );

        VectorLifecycle::iterator it_found = StdAlgorithm::find_if( m_lifecycles.begin(), m_lifecycles.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } );

        if( it_found != m_lifecycles.end() )
        {
            m_lifecycles.erase( it_found );

            return;
        }

        VectorLifecycle::iterator it_add_found = StdAlgorithm::find_if( m_lifecyclesAdd.begin(), m_lifecyclesAdd.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } );

        if( it_add_found != m_lifecyclesAdd.end() )
        {
            m_lifecyclesAdd.erase( it_add_found );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::preUpdate()
    {
        if( m_lifecyclesAdd.empty() == false )
        {
            m_lifecycles.insert( m_lifecycles.end(), m_lifecyclesAdd.begin(), m_lifecyclesAdd.end() );
            m_lifecyclesAdd.clear();
        }

        for( const LifecycleDesc & desc : m_lifecycles )
        {
            desc.lifecycle->preUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::update()
    {
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            desc.lifecycle->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::postUpdate()
    {
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            desc.lifecycle->postUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

