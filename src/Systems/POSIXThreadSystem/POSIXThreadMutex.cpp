#include "POSIXThreadMutex.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadMutex::POSIXThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXThreadMutex::~POSIXThreadMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadMutex::initialize()
    {
        pthread_mutexattr_t attr;
        int attr_status = ::pthread_mutexattr_init( &attr );

        if( attr_status != 0 )
        {
            LOGGER_ERROR( "invalid init mutex attr error: %d (doc: %s)"
                , attr_status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );

            return false;
        }

        ::pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

        int status = ::pthread_mutex_init( &m_mutex, &attr );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid create mutex error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );

            ::pthread_mutexattr_destroy( &attr );

            return false;
        }

        ::pthread_mutexattr_destroy( &attr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadMutex::finalize()
    {
        int status = ::pthread_mutex_destroy( &m_mutex );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid destroy mutex error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadMutex::lock()
    {
        int status = ::pthread_mutex_lock( &m_mutex );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid lock error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadMutex::unlock()
    {
        int status = ::pthread_mutex_unlock( &m_mutex );

        if( status != 0 )
        {
            LOGGER_ERROR( "invalid unlock error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXThreadMutex::tryLock()
    {
        int status = ::pthread_mutex_trylock( &m_mutex );

        if( status == 0 )
        {
            return true;
        }

        if( status != EBUSY )
        {
            LOGGER_ERROR( "invalid try lock error: %d (doc: %s)"
                , status
                , MENGINE_DOCUMENT_STR( this->getDocument() )
            );

            return false;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXThreadMutex::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
