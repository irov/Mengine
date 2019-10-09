#include "Factory.h"
#include "Factorable.h"

#include "Interface/FactoryServiceInterface.h"

#ifdef MENGINE_DEBUG
#   include "Interface/NotificationServiceInterface.h"
#   include "Kernel/Logger.h"
#   include <stdlib.h>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Factory::Factory( const Char * _name )
        : m_name( _name )
        , m_count( 0 )
    {
#ifdef MENGINE_DEBUG
        FACTORY_SERVICE()
            ->registerFactory( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    Factory::~Factory()
    {
#ifdef MENGINE_DEBUG
        FACTORY_SERVICE()
            ->unregisterFactory( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Factory::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer Factory::createObject( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        STDEX_THREAD_GUARD_CHECK( this, "Factory::createObject" );

        ++m_count;
        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        Factorable * object = this->_createObject();
        object->setFactory( this );

#ifdef MENGINE_DEBUG
        FACTORY_SERVICE()
            ->debugFactoryCreateObject( this, object, _doc );

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
        STDEX_THREAD_GUARD_CHECK( this, "Factory::destroyObject" );

#ifdef MENGINE_DEBUG
        FACTORY_SERVICE()
            ->debugFactoryDestroyObject( this, _object );

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT, this, _object );
        }
#endif

        this->_destroyObject( _object );

        --m_count;
        IntrusivePtrBase::intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Factory::isEmptyObjects() const
    {
        return m_count == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Factory::getCountObject() const
    {
        return m_count;
    }
    //////////////////////////////////////////////////////////////////////////
    void Factory::destroy()
    {
        this->_destroy();

        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
}
