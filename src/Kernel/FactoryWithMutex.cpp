#include "FactoryWithMutex.h"

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"

#ifdef MENGINE_DEBUG
#   include "Kernel/NotificationHelper.h"
#   include "Kernel/Logger.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::FactoryWithMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::~FactoryWithMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & FactoryWithMutex::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer FactoryWithMutex::createObject( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_type.empty() == false );

        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object );

        m_count.incref();

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

#ifdef MENGINE_DEBUG
        m_factorables.push_back( object );
#endif

        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }

        object->setFactory( this );

#ifdef MENGINE_DOCUMENT_ENABLE
        object->setDocument( _doc );
#endif

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->debugFactoryCreateObject( this, object );
        }
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->debugFactoryDestroyObject( this, _object );
        }
#endif

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

#ifdef MENGINE_DEBUG
        m_factorables.remove( _object );
#endif

        this->_destroyObject( _object );

        m_count.decref();

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
}
