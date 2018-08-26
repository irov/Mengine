#include "FactoryWithMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithMutex::FactoryWithMutex( const char * _name )
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
    PointerFactorable FactoryWithMutex::createObject()
    {
        m_mutex->lock();

        ++m_count;
        intrusive_ptr_add_ref( this );

        Factorable * object = this->_createObject();
        object->setFactory( this );

        m_mutex->unlock();

        return object;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithMutex::destroyObject( Factorable * _object )
    {
        m_mutex->lock();

        this->_destroyObject( _object );

        --m_count;
        intrusive_ptr_dec_ref( this );

        m_mutex->unlock();
    }
}
