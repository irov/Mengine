#include "Factory.h"
#include "Factorable.h"

#include "Interface/FactoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

#ifdef MENGINE_DEBUG
#   include "Interface/NotificationServiceInterface.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factory::Factory()
#ifdef MENGINE_DEBUG
        : m_register( false )
#endif
    {
#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( FactoryServiceInterface ) == true )
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
#ifdef MENGINE_DEBUG
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
    FactorablePointer Factory::createObject( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_type.empty() == false );

        Factorable * object = this->_createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( object );

        m_count.incref();

        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        object->setFactory( this );

#if MENGINE_DOCUMENT_ENABLE
        object->setDocument( _doc );
#endif

#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->debugFactoryCreateObject( this, object, _doc );
        }

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_FACTORY_CREATE_OBJECT, this, object, _doc );
        }
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroyObject( Factorable * _object )
    {
#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->debugFactoryDestroyObject( this, _object );
        }

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT, this, _object );
        }
#endif

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr doc = _object->getDocument();
        MENGINE_UNUSED( doc );

        _object->setDocument( nullptr );
#endif

        this->_destroyObject( _object );

        m_count.decref();

        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factory::isEmptyObjects() const
    {
        bool referencing = m_count.isReferencing();

        return referencing == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factory::getCountObject() const
    {
        uint32_t count = m_count.getReferenceCount();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
}
