#include "NotificationService.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/MixinDebug.h"

#include "Config/Algorithm.h"

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
                ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex );

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

        uint32_t index = 0;
        for( VectorObservers & observers : m_observers )
        {
#if MENGINE_DOCUMENT_ENABLE
            for( const ObserverDesc & desc : observers )
            {
                if( desc.dead == true )
                {
                    continue;
                }

                LOGGER_MESSAGE( "Notification '%s' not clear"
                    , MENGINE_DOCUMENT_STR( desc.doc )
                );
            }
#endif

            MENGINE_ASSERTION( std::find_if( observers.begin(), observers.end(), []( const ObserverDesc & _desc )
            {
                return _desc.dead == false;
            } ) == observers.end(), "finalized notification '%u' has observers"
                , index
                );

            observers.clear();

            ++index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver( uint32_t _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        LOGGER_INFO( "notify", "add observer '%s' id [%u] uid [%u] p [%p]"
            , MENGINE_MIXIN_DEBUG_NAME( _observer )
            , _id
            , MENGINE_MIXIN_DEBUG_UID( _observer )
            , _observer
        );

        MENGINE_ASSERTION_FATAL( this->hasObserver_( _id, _observer ) == false, "dublicate observer '%s' id [%u] (doc: %s)"
            , MENGINE_MIXIN_DEBUG_NAME( _observer )
            , _id
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( m_visiting != 0 )
        {
            ObserverQueue desc;
            desc.id = _id;
            desc.observer = _observer;
            desc.callable = _callable;

#if MENGINE_DOCUMENT_ENABLE
            desc.doc = _doc;
#endif

            m_add.emplace_back( desc );

            return;
        }

        this->addObserver_( _id, _observer, _callable, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver( uint32_t _id, Observable * _observer )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        LOGGER_INFO( "notify", "remove observer '%s' id [%u] uid [%u] p [%p]"
            , MENGINE_MIXIN_DEBUG_NAME( _observer )
            , _id
            , MENGINE_MIXIN_DEBUG_UID( _observer )
            , _observer
        );

        for( VectorObserverQueues::iterator
            it = m_add.begin(),
            it_end = m_add.end();
            it != it_end;
            ++it )
        {
            const ObserverQueue & queue = *it;

            if( queue.id != _id )
            {
                continue;
            }

            if( queue.observer != _observer )
            {
                continue;
            }

            //keep callable ptr
            ObserverCallableInterfacePtr keep_callable = queue.callable;

            *it = m_add.back();
            m_add.pop_back();

            return;
        }

        if( m_visiting != 0 )
        {
            VectorObservers & observers = m_observers[_id];

            for( ObserverDesc & desc : observers )
            {
                if( desc.dead == true )
                {
                    continue;
                }

                if( desc.observer != _observer )
                {
                    continue;
                }

                ObserverCallableInterfacePtr keep_callable = desc.callable;
                desc.callable = nullptr;

                desc.dead = true;

                return;
            }

            MENGINE_ASSERTION( false, "alredy remove observer for notificator %u"
                , _id
            );

            return;
        }

        this->removeObserver_( _id, _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::hasObserver( Observable * _observer ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( const VectorObservers & observers : m_observers )
        {
            for( const ObserverDesc & desc : observers )
            {
                if( desc.observer != _observer )
                {
                    continue;
                }

                if( desc.dead == true )
                {
                    return false;
                }

                return true;
            }
        }

        for( const ObserverQueue & queue : m_add )
        {
            if( queue.observer != _observer )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::visitObservers( uint32_t _id, const LambdaObserver & _lambda )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        this->updateObservers_( _id );

        const VectorObservers & observers = m_observers[_id];

        if( observers.empty() == true )
        {
            return true;
        }

        bool successful = true;

        ++m_visiting;

        for( const ObserverDesc & desc : observers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            try
            {
                _lambda( desc.callable );
            }
            catch( const ExceptionNotificationFailed & )
            {
                successful = false;
            }
        }

        MENGINE_ASSERTION_FATAL( m_visiting != 0 );

        --m_visiting;

        this->updateObservers_( _id );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::hasObserver_( uint32_t _id, Observable * _observer ) const
    {
        const VectorObservers & observers = m_observers[_id];

        for( const ObserverDesc & desc : observers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            if( desc.dead == true )
            {
                return false;
            }

            return true;
        }

        for( const ObserverQueue & queue : m_add )
        {
            if( queue.id != _id )
            {
                continue;
            }

            if( queue.observer != _observer )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::updateObservers_( uint32_t _id )
    {
        if( m_visiting != 0 )
        {
            return;
        }

        VectorObserverQueues move_add = std::move( m_add );

        for( const ObserverQueue & q : move_add )
        {
            this->addObserver_( q.id, q.observer, q.callable, MENGINE_DOCUMENT_VALUE( q.doc, nullptr ) );
        }

        ++m_visiting;

        VectorObservers & observers_for_erase = m_observers[_id];

        observers_for_erase.erase( std::remove_if( observers_for_erase.begin(), observers_for_erase.end(), []( const ObserverDesc & _desc )
        {
            return _desc.dead;
        } ), observers_for_erase.end() );

        MENGINE_ASSERTION_FATAL( m_visiting != 0 );

        --m_visiting;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver_( uint32_t _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        VectorObservers & observers = m_observers[_id];

        ObserverDesc desc;
        desc.observer = _observer;
        desc.callable = _callable;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        desc.dead = false;

        observers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver_( uint32_t _id, Observable * _observer )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT );

        VectorObservers & observers = m_observers[_id];

        for( ObserverDesc & desc : observers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            if( desc.dead == true )
            {
                continue;
            }

            //keep callable ptr
            ObserverCallableInterfacePtr keep_callable = desc.callable;

            desc = observers.back();
            observers.pop_back();
            return;
        }

        MENGINE_ASSERTION( false, "alredy remove observer for notificator %u"
            , _id
        );
    }
    //////////////////////////////////////////////////////////////////////////
}