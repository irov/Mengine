#include "SemaphoreService.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/ThreadHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SemaphoreService, Mengine::SemaphoreService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SemaphoreService::SemaphoreService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SemaphoreService::~SemaphoreService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SemaphoreService::_initializeService()
    {
        m_factorySemaphore = Helper::makeFactoryPool<Semaphore, 8>( MENGINE_DOCUMENT_FACTORABLE );

        m_mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SemaphoreService::_finalizeService()
    {
        for( const HashtableSemaphores::value_type & value : m_semaphores )
        {
            const SemaphorePtr & semaphore = value.element;

            semaphore->finalize();
        }

        m_semaphores.clear();

        m_mutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySemaphore );

        m_factorySemaphore = nullptr;
     }
    //////////////////////////////////////////////////////////////////////////
    void SemaphoreService::activateSemaphore( const ConstString & _name )
    {
        m_mutex->lock();

        const SemaphorePtr & semaphore = m_semaphores.find( _name );

        if( semaphore == SemaphorePtr::none() )
        {
            SemaphorePtr new_semaphore = m_factorySemaphore->createObject( MENGINE_DOCUMENT_FACTORABLE );

            new_semaphore->initalize( true );

            m_semaphores.emplace( _name, new_semaphore );

            m_mutex->unlock();

            return;
        }

        if( semaphore->isActivated() == true )
        {
            m_mutex->unlock();

            return;
        }

        VectorSemaphoreListeners copy_listeners = semaphore->activate();

        m_mutex->unlock();

        Helper::dispatchMainThreadEvent( [copy_listeners]()
        {
            for( const SemaphoreListenerInterfacePtr & listener : copy_listeners )
            {
                listener->onActivated();
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void SemaphoreService::deactivateSemaphore( const ConstString & _name )
    {
        m_mutex->lock();

        SemaphorePtr semaphore = m_semaphores.erase( _name );

        if( semaphore == nullptr )
        {
            m_mutex->unlock();

            return;
        }

        semaphore->finalize();

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void SemaphoreService::waitSemaphore( const ConstString & _name, const SemaphoreListenerInterfacePtr & _listener )
    {
        m_mutex->lock();

        const SemaphorePtr & semaphore = m_semaphores.find( _name );

        if( semaphore == SemaphorePtr::none() )
        {
            SemaphorePtr new_semaphore = m_factorySemaphore->createObject( MENGINE_DOCUMENT_FACTORABLE );

            new_semaphore->initalize( false );
            new_semaphore->addListener( _listener );

            m_semaphores.emplace( _name, new_semaphore );

            m_mutex->unlock();

            return;
        }

        if( semaphore->isActivated() == false )
        {
            semaphore->addListener( _listener );

            m_mutex->unlock();

            return;
        }

        m_mutex->unlock();

        SemaphoreListenerInterfacePtr copy_listener = _listener;

        Helper::dispatchMainThreadEvent( [copy_listener]()
        {
            copy_listener->onActivated();
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}
