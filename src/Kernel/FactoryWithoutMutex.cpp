#include "FactoryWithoutMutex.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/AssertionMemoryPanic.h"
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

        MENGINE_ASSERTION_FATAL( m_type.empty() == false, "factory type not set" );

        MENGINE_THREAD_GUARD_SCOPE( FactoryWithoutMutex, this );

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object, "invalid create object '%s' doc '%s'"
            , m_type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        m_count.increfReferenceCount();

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

#if defined(MENGINE_DEBUG)
        m_factorables.push_back( object );
#endif

        object->setFactory( this );

        UniqueId id = Helper::generateUniqueIdentity();
        object->setUniqueIdentity( id );

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        Timestamp timestamp = Helper::getSystemTimestamp();
        object->setFactorableTimestamp( timestamp );
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
        object->setDocument( _doc );
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithoutMutex::destroyObject( Factorable * _object )
    {
        MENGINE_THREAD_GUARD_BEGIN( FactoryWithoutMutex, this, "FactoryWithoutMutex::destroyObject" );

#if defined(MENGINE_DEBUG)
        m_factorables.remove( _object );
#endif

        this->_destroyObject( _object );

        m_count.decrefReferenceCount();

        MENGINE_THREAD_GUARD_END( FactoryWithoutMutex, this, "FactoryWithoutMutex::destroyObject" );

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
