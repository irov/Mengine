#include "FactoryWithMutex.h"

#ifdef MENGINE_DEBUG
#   include "Interface/NotificationServiceInterface.h"
#   include "Kernel/Logger.h"
#   include <stdlib.h>
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
    FactorablePointer FactoryWithMutex::createObject( const Char * _doc )
    {
		MENGINE_UNUSED( _doc );

        m_mutex->lock();

        ++m_count;
        IntrusivePtrBase::intrusive_ptr_add_ref( this );

        Factorable * object = this->_createObject();
        object->setFactory( this );

        m_mutex->unlock();

#ifdef MENGINE_DEBUG
		NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CREATE_OBJECT, (Factory *)this, object, _doc );
#endif

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
#ifdef MENGINE_DEBUG
		NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_DESTROY_OBJECT, (Factory *)this, _object );
#endif

        m_mutex->lock();

        this->_destroyObject( _object );

        --m_count;
        IntrusivePtrBase::intrusive_ptr_dec_ref( this );

        m_mutex->unlock();
    }
}
