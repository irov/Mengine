#include "Factory.h"
#include "Factorable.h"

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/NotificationHelper.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factory::Factory()
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
    Factory::~Factory()
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
    bool Factory::initialize( const ConstString & _type )
    {
        m_type = _type;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::finalize()
    {
        m_type.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer Factory::createObject( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_type.empty() == false );

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object );

        m_count.incref();

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        object->setFactory( this );

#if defined(MENGINE_DOCUMENT_ENABLE)
        object->setDocument( _doc );
#endif

#if defined(MENGINE_DEBUG)
        m_factorables.push_back( object );
#endif

//#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
//        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
//        {
//            FACTORY_SERVICE()
//                ->debugFactoryCreateObject( this, object );
//        }
//#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroyObject( Factorable * _object )
    {
//#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
//        if( SERVICE_IS_INITIALIZE( FactoryServiceInterface ) == true )
//        {
//            FACTORY_SERVICE()
//                ->debugFactoryDestroyObject( this, _object );
//        }
//#endif

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

        m_count.decref();

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factory::isEmptyObjects() const
    {
        uint32_t counter = m_count.getReferenceCount();

        if( counter != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factory::getCountObject() const
    {
        uint32_t count = m_count.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        MENGINE_UNUSED( _mutex );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & Factory::getMutex() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return ThreadMutexInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
    //////////////////////////////////////////////////////////////////////////
    void Factory::foreachFactorables( const LambdaFactorable & _lambda )
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
