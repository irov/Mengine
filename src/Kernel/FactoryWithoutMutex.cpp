#include "FactoryWithoutMutex.h"

#include "ThreadGuardScope.h"
#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithoutMutex::FactoryWithoutMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryWithoutMutex::~FactoryWithoutMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer FactoryWithoutMutex::createObject( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_THREAD_GUARD_SCOPE( FactoryWithoutMutex, this, "FactoryWithoutMutex::createObject" );

        MENGINE_ASSERTION_FATAL( m_type.empty() == false );

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object );

        m_count.incref();

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

#if defined(MENGINE_DEBUG)
        m_factorables.push_back( object );
#endif

        object->setFactory( this );

#if defined(MENGINE_DOCUMENT_ENABLE)
        object->setDocument( _doc );
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithoutMutex::destroyObject( Factorable * _object )
    {
        MENGINE_THREAD_GUARD_SCOPE( FactoryWithoutMutex, this, "FactoryWithoutMutex::destroyObject" );

#if defined(MENGINE_DEBUG)
        m_factorables.remove( _object );
#endif

        this->_destroyObject( _object );

        m_count.decref();

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithoutMutex::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        MENGINE_UNUSED( _mutex );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & FactoryWithoutMutex::getMutex() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return ThreadMutexInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
}
