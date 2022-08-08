#include "NotificationArea.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/MixinDebug.h"

#include "Config/Algorithm.h"

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
    bool NotificationArea::initialize( uint32_t _id )
    {
        m_id = _id;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::finalize()
    {
        m_addObservers.clear();

#ifdef MENGINE_DOCUMENT_ENABLE
        for( const ObserverDesc & observer : m_observers )
        {
            if( observer.callable == nullptr )
            {
                continue;
            }

            LOGGER_MESSAGE( "Notification '%s' not clear"
                , MENGINE_DOCUMENT_STR( observer.doc )
            );
        }
#endif

        MENGINE_ASSERTION( Algorithm::find_if( m_observers.begin(), m_observers.end(), []( const ObserverDesc & _desc )
        {
            return _desc.observer != nullptr;
        } ) == m_observers.end(), "finalized notification '%u' has observers"
            , m_id
            );

        m_observers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::addObserver( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MENGINE_ASSERTION_FATAL( this->hasObserver_( _observer ) == false, "dublicate observer '%s' id [%u] (doc: %s)"
            , MENGINE_MIXIN_DEBUG_NAME( _observer )
            , m_id
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( m_visiting != 0 )
        {
            ObserverDesc desc;
            desc.observer = _observer;
            desc.callable = _callable;

#ifdef MENGINE_DOCUMENT_ENABLE
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
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        for( VectorObservers::iterator
            it = m_addObservers.begin(),
            it_end = m_addObservers.end();
            it != it_end;
            ++it )
        {
            const ObserverDesc & queue = *it;

            if( queue.observer != _observer )
            {
                continue;
            }

            //keep callable ptr
            ObserverCallableInterfacePtr keep_callable = queue.callable;

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
                desc.callable = nullptr;

                return;
            }

            MENGINE_ASSERTION( false, "alredy remove observer for notificator %u"
                , m_id
            );

            return;
        }

        this->removeObserver_( _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationArea::hasObserver( Observable * _observer ) const
    {
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

        MENGINE_ASSERTION_FATAL( m_visiting != 0 );

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

        VectorObservers move_add = std::move( m_addObservers );

        for( const ObserverDesc & desc : move_add )
        {
            this->addObserver_( desc.observer, desc.callable, MENGINE_DOCUMENT_VALUE( desc.doc, nullptr ) );
        }

        ++m_visiting;

        m_addObservers.erase( Algorithm::remove_if( m_addObservers.begin(), m_addObservers.end(), []( const ObserverDesc & _desc )
        {
            return _desc.observer == nullptr;
        } ), m_addObservers.end() );

        MENGINE_ASSERTION_FATAL( m_visiting != 0 );

        --m_visiting;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationArea::addObserver_( Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        ObserverDesc desc;
        desc.observer = _observer;
        desc.callable = _callable;

#ifdef MENGINE_DOCUMENT_ENABLE
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

        MENGINE_ASSERTION( false, "alredy remove observer for notificator %u"
            , m_id
        );
    }
    //////////////////////////////////////////////////////////////////////////
}