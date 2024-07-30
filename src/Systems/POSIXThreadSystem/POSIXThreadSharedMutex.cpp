#include "POSIXThreadSharedMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadSharedMutex::POSIXThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadSharedMutex::~POSIXThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadSharedMutex::initialize()
    {
        int status = pthread_rwlock_init( &m_rwlock, nullptr );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid create rwlock error status: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::finalize()
    {
        int status = pthread_rwlock_destroy( &m_rwlock );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid destroy rwlock error status: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::lock()
    {
        int status = pthread_rwlock_wrlock( &m_rwlock );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid lock error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::unlock()
    {
        int status = pthread_rwlock_unlock( &m_rwlock );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid unlock error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadSharedMutex::tryLock()
    {
        int status = pthread_rwlock_trywrlock( &m_rwlock );

        if( status == 0 )
        {
            return true;
        }

        if( status == EBUSY )
        {
            return false;
        }

        LOGGER_ERROR( "invalid try lock error: %d (doc: %s)"
            , status
            , MENGINE_DOCUMENT_STR( this->getDocument() )
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::lockShared()
    {
        int status = pthread_rwlock_rdlock( &m_rwlock );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid lock shared error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::unlockShared()
    {
        int status = pthread_rwlock_unlock( &m_rwlock );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid unlock shared error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadSharedMutex::tryLockShared()
    {
        int status = pthread_rwlock_tryrdlock( &m_rwlock );

        if( status == 0 )
        {
            return true;
        }

        if( status == EBUSY )
        {
            return false;
        }

        LOGGER_ERROR( "invalid try lock error: %d (doc: %s)"
            , status
            , MENGINE_DOCUMENT_STR( this->getDocument() )
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadSharedMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
