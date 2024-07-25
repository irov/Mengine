#include "FactoryWithMutex.h"

#include "Kernel/ThreadMutexScope.h"

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
    FactorablePointer FactoryWithMutex::createObject( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_type.empty() == false, "factory type not set" );

        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object );        

#if defined(MENGINE_DEBUG)
        m_factorables.push_back( object );
#endif

        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }

        m_count.incref();

        object->setFactory( this );

#if defined(MENGINE_DOCUMENT_ENABLE)
        object->setDocument( _doc );
#endif

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }

#if defined(MENGINE_DEBUG)
        m_factorables.remove( _object );
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr doc = _object->getDocument();
        MENGINE_UNUSED( doc );

#if defined(MENGINE_RELEASE)
        _object->setDocument( nullptr );
#endif
#endif

        this->_destroyObject( _object );

        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }

        m_count.decref();        

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
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
}
