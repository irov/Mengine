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
        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex, false );

            m_mutex = mutex;

            return true;
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            m_mutex = nullptr;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::_finalizeService()
    {
        m_add.clear();
        m_remove.clear();

        uint32_t index = 0;
        for( VectorObservers & observers : m_mapObserves )
        {
#ifdef MENGINE_DEBUG
            for( const ObserverDesc & desc : observers )
            {
                LOGGER_MESSAGE( "Notification '%s' not clear"
                    , desc.doc.c_str()
                );
            }
#endif

            MENGINE_ASSERTION( observers.empty() == true, "finalized notification '%d' has observers", index );

            observers.clear();

            ++index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable, const Char * _doc )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( m_visiting != 0 )
        {
            ObserverQueue desc;
            desc.id = _id;
            desc.observer = _observer;
            desc.callable = _callable;

#ifdef MENGINE_DEBUG
            desc.doc = _doc;
#endif

            m_add.emplace_back( desc );

            return;
        }

        this->addObserver_( _id, _observer, _callable, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver( uint32_t _id, const ObservablePtr & _observer )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

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
        MENGINE_ASSERTION_FATAL_RETURN( _id < MENGINE_NOTIFICATOR_MAX_COUNT, false );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );        

        const VectorObservers & observers = m_mapObserves[_id];

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
#ifdef MENGINE_DEBUG
                this->addObserver_( q.id, q.observer, q.callable, q.doc.c_str() );
#else
                this->addObserver_( q.id, q.observer, q.callable, nullptr );
#endif
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
    void NotificationService::addObserver_( uint32_t _id, const ObservablePtr & _observer, const ObserverCallableInterfacePtr & _callable, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        VectorObservers & observers = m_mapObserves[_id];

        ObserverDesc desc;
        desc.observer = _observer;
        desc.callable = _callable;
        
#ifdef MENGINE_DEBUG
        desc.doc = _doc;
#endif

        observers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver_( uint32_t _id, const ObservablePtr & _observer )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        VectorObservers & observers = m_mapObserves[_id];

        MENGINE_ASSERTION( observers.empty() == false );

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