#include "NotificationService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ThreadMutexScope.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NotificationService, Mengine::NotificationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NotificationService::NotificationService()
        : m_visiting( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationService::~NotificationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::_initializeService()
    {
        SERVICE_WAIT( Mengine::ThreadServiceInterface, [this]() {
            ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( mutex );

            m_mutex = mutex;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::_finalizeService()
    {
        m_add.clear();
        m_remove.clear();

        for( MapObservers::const_iterator
            it = m_mapObserves.begin(),
            it_end = m_mapObserves.end();
            it != it_end;
            ++it )
        {
            const VectorObservers & observers = it->second;

            (void)observers;

            MENGINE_ASSERTION( observers.empty() == true, ("finalized notification service has observers") );
        }

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_visiting != 0 )
        {
            ObserverQueue desc;
            desc.id = _id;
            desc.observer = _observer;
            desc.callable = _callable;

            m_add.emplace_back( desc );

            return;
        }

        this->addObserver_( _id, _observer, _callable );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver( uint32_t _id, const ObservablePtr & _observer )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_visiting != 0 )
        {
            ObserverQueue desc;
            desc.id = _id;
            desc.observer = _observer;

            m_remove.emplace_back( desc );

            return;
        }

        this->removeObserver_( _id, _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::visitObservers( uint32_t _id, const LambdaObserver & _lambda )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapObservers::const_iterator it_find = m_mapObserves.find( _id );

        if( it_find == m_mapObserves.end() )
        {
            return true;
        }

        const VectorObservers & observers = it_find->second;

        bool successful = true;

        ++m_visiting;

        for( const ObserverDesc & desc : observers )
        {
            try
            {
                _lambda( desc.callable );
            }
            catch( const ExceptionNotificationFailed & )
            {
                successful = false;
            }
        }

        --m_visiting;

        if( m_visiting == 0 )
        {
            for( const ObserverQueue & q : m_add )
            {
                this->addObserver_( q.id, q.observer, q.callable );
            }

            m_add.clear();

            for( const ObserverQueue & q : m_remove )
            {
                this->removeObserver_( q.id, q.observer );
            }

            m_remove.clear();
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver_( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable )
    {
        MapObservers::iterator it_find = m_mapObserves.find( _id );

        if( it_find == m_mapObserves.end() )
        {
            VectorObservers new_observers;

            it_find = m_mapObserves.emplace( _id, new_observers ).first;
        }

        VectorObservers & observers = it_find->second;

        ObserverDesc desc;
        desc.observer = _observer;
        desc.callable = _callable;

        observers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver_( uint32_t _id, const ObservablePtr & _observer )
    {
        MapObservers::iterator it_find = m_mapObserves.find( _id );

        if( it_find == m_mapObserves.end() )
        {
            return;
        }

        VectorObservers & observers = it_find->second;

        for( ObserverDesc & desc : observers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            desc = observers.back();
            observers.pop_back();
            break;
        }
    }
}