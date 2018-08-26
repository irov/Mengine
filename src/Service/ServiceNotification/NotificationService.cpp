#include "NotificationService.h"

#include "Kernel/Assertion.h"

#include "Kernel/Logger.h"

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
        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        if( mutex == nullptr )
        {
            return false;
        }

        m_mutex = mutex;

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

            MENGINE_ASSERTION( observers.empty() == true );
        }

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable )
    {
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
    void NotificationService::visitObservers( uint32_t _id, ObserverVisitorCallableInterface * _visitor )
    {
        MapObservers::iterator it_find = m_mapObserves.find( _id );

        if( it_find == m_mapObserves.end() )
        {
            return;
        }

        const VectorObservers & observers = it_find->second;

        m_mutex->lock();

        ++m_visiting;

        for( const ObserverDesc & desc : observers )
        {
            _visitor->visit( desc.callable );
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

        m_mutex->unlock();
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
        }
    }
}