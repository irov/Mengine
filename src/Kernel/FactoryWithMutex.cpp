#include "FactoryWithMutex.h"

#include "Interface/FactoryServiceInterface.h"

#ifdef MENGINE_DEBUG
#   include "Interface/NotificationServiceInterface.h"
#   include "Kernel/Logger.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::FactoryWithMutex( const Char * _name )
        : Factory( _name )
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

        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }

        ++m_count;
        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        Factorable * object = this->_createObject();
        
        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }

        object->setFactory( this );

#ifdef MENGINE_DEBUG
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
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_FACTORY_CREATE_OBJECT, (Factory *)this, object, _doc );
        }
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
#ifdef MENGINE_DEBUG
        if( SERVICE_EXIST( FactoryServiceInterface ) == true )
        {
            FACTORY_SERVICE()
                ->debugFactoryDestroyObject( this, _object );
        }

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT, (Factory *)this, _object );
        }
#endif
        if( m_mutex != nullptr )
        {
            m_mutex->lock();
        }

        this->_destroyObject( _object );

        --m_count;
        IntrusivePtrBase::intrusive_ptr_dec_ref( this );

        if( m_mutex != nullptr )
        {
            m_mutex->unlock();
        }
    }
}
