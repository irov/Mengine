#include "FactoryWithMutex.h"

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/ThreadMutexScope.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/NotificationHelper.h"
#   include "Kernel/Logger.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::FactoryWithMutex()
#if defined(MENGINE_DEBUG)
        : m_register( false )
#endif
    {
#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->registerFactory( this );

            m_register = true;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::~FactoryWithMutex()
    {
#if defined(MENGINE_DEBUG)
        if( m_register == true )
        {
            FACTORY_SERVICE()
                ->unregisterFactory( this );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryWithMutex::initialize( const ConstString & _type )
    {
        m_type = _type;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::finalize()
    {
        m_type.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer FactoryWithMutex::createObject( const DocumentInterfacePtr & _doc )
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

#if defined(MENGINE_DEBUG)
        m_factorables.push_back( object );
#endif

        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }

        object->setFactory( this );

#if defined(MENGINE_DOCUMENT_ENABLE)
        object->setDocument( _doc );
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

#if defined(MENGINE_DEBUG)
        m_factorables.remove( _object );
#endif

        this->_destroyObject( _object );

        m_count.decref();

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FactoryWithMutex::isEmptyObjects() const
    {
        uint32_t counter = m_count.getReferenceCount();

        if( counter != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FactoryWithMutex::getCountObject() const
    {
        uint32_t count = m_count.getReferenceCount();

        return count;
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
#if defined(MENGINE_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::foreachFactorables( const LambdaFactorable & _lambda )
    {
        for( Factorable * factorable : m_factorables )
        {
            if( _lambda( factorable ) == false )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
