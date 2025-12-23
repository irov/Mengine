#include "DispatcherService.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( DispatcherService, Mengine::DispatcherService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DispatcherService::DispatcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DispatcherService::~DispatcherService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DispatcherService::registerService( ServiceInterface * _service )
    {
        DispatchableInterface * dispatchable = dynamic_cast<DispatchableInterface *>(_service);

        if( dispatchable == nullptr )
        {
            return;
        }

        this->addDispatchable( dispatchable );
    }
    //////////////////////////////////////////////////////////////////////////
    void DispatcherService::unregisterService( ServiceInterface * _service )
    {
        DispatchableInterface * dispatchable = dynamic_cast<DispatchableInterface *>(_service);
        
        if( dispatchable == nullptr )
        {
            return;
        }

        this->removeDispatchable( dispatchable );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DispatcherService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DispatcherService::_finalizeService()
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
    void DispatcherService::addDispatchable( DispatchableInterface * _dispatchable )
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
    void DispatcherService::removeDispatchable( DispatchableInterface * _dispatchable )
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
    void DispatcherService::preUpdate()
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
    void DispatcherService::update()
    {
        for( const DispatchableDesc & desc : m_dispatchable )
        {
            desc.dispatchable->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DispatcherService::postUpdate()
    {
        for( const DispatchableDesc & desc : m_dispatchable )
        {
            desc.dispatchable->postUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

