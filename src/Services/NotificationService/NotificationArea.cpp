#include "NotificationArea.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/NotificationName.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NotificationArea::NotificationArea()
        : m_id( 0 )
        , m_visiting( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationArea::~NotificationArea()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & NotificationArea::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationArea::initialize( NotificatorId _id )
    {
        m_id = _id;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::finalize()
    {
        m_addObservers.clear();

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const ObserverDesc & observer : m_observers )
        {
            if( observer.observer == nullptr )
            {
                continue;
            }

            MENGINE_ASSERTION_FATAL( false, "notification '%s' [%u] not clear observer (doc: %s)"
                , Helper::getNotificationName( m_id )
                , m_id
                , MENGINE_DOCUMENT_STR( observer.doc )
            );
        }
#endif

        m_observers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::addObserver( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _observer, "invalid add observer nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( _callable, "invalid add observer callable nullptr" );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MENGINE_ASSERTION_FATAL( this->hasObserver_( _observer ) == false, "dublicate observer '%s' notificator '%s' [%u] (doc: %s)"
            , MENGINE_MIXIN_DEBUG_NAME( _observer )
            , Helper::getNotificationName( m_id )
            , m_id
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( m_visiting != 0 )
        {
            ObserverDesc desc;
            desc.observer = _observer;
            desc.callable = _callable;

#if defined(MENGINE_DOCUMENT_ENABLE)
            desc.doc = _doc;
#endif

            m_addObservers.emplace_back( desc );

            return;
        }

        this->addObserver_( _observer, _callable, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::removeObserver( Observable * _observer )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _observer, "invalid remove observer nullptr" );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( VectorObservers::iterator
            it = m_addObservers.begin(),
            it_end = m_addObservers.end();
            it != it_end;
            ++it )
        {
            const ObserverDesc & desc = *it;

            if( desc.observer != _observer )
            {
                continue;
            }

            ObserverCallableInterfacePtr keep_callable = desc.callable;

            *it = m_addObservers.back();
            m_addObservers.pop_back();

            return;
        }

        if( m_visiting != 0 )
        {
            for( ObserverDesc & desc : m_observers )
            {
                if( desc.observer != _observer )
                {
                    continue;
                }

                ObserverCallableInterfacePtr keep_callable = desc.callable;
                desc.observer = nullptr;

                return;
            }

            MENGINE_ASSERTION( false, "already remove observer for notificator %s [%u]"
                , Helper::getNotificationName( m_id )
                , m_id
            );

            return;
        }

        this->removeObserver_( _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationArea::hasObserver( Observable * _observer ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _observer, "invalid has observer nullptr" );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        bool exist = this->hasObserver_( _observer );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationArea::foreachObservers( const NotificationServiceInterface::LambdaObserver & _lambda )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        this->updateObservers_();

        if( m_observers.empty() == true )
        {
            return true;
        }

        bool successful = true;

        ++m_visiting;

        for( const ObserverDesc & desc : m_observers )
        {
            if( desc.observer == nullptr )
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

        MENGINE_ASSERTION_FATAL( m_visiting != 0, "invalid visiting" );

        --m_visiting;

        this->updateObservers_();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationArea::hasObserver_( Observable * _observer ) const
    {
        for( const ObserverDesc & desc : m_observers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            return true;
        }

        for( const ObserverDesc & desc : m_addObservers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::updateObservers_()
    {
        if( m_visiting != 0 )
        {
            return;
        }

        if( m_addObservers.empty() == false )
        {
            for( const ObserverDesc & desc : m_addObservers )
            {
                this->addObserver_( desc.observer, desc.callable, MENGINE_DOCUMENT_VALUE( desc.doc, nullptr ) );
            }

            m_addObservers.clear();
        }

        if( m_observers.empty() == false )
        {
            ++m_visiting;

            m_observers.erase( StdAlgorithm::remove_if( m_observers.begin(), m_observers.end(), []( const ObserverDesc & _desc )
            {
                return _desc.observer == nullptr;
            } ), m_observers.end() );

            MENGINE_ASSERTION_FATAL( m_visiting != 0, "invalid visiting" );

            --m_visiting;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::addObserver_( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        ObserverDesc desc;
        desc.observer = _observer;
        desc.callable = _callable;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_observers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::removeObserver_( Observable * _observer )
    {
        for( ObserverDesc & desc : m_observers )
        {
            if( desc.observer != _observer )
            {
                continue;
            }

            //keep callable ptr
            ObserverCallableInterfacePtr keep_callable = desc.callable;

            desc = m_observers.back();
            m_observers.pop_back();

            return;
        }

        MENGINE_ASSERTION( false, "already remove observer for notificator [%s] %u"
            , Helper::getNotificationName( m_id )
            , m_id
        );
    }
    //////////////////////////////////////////////////////////////////////////
}