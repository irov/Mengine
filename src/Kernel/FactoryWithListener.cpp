#include "FactoryWithListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithListener::FactoryWithListener( const char * _name )
        : Factory( _name )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactoryWithListener::~FactoryWithListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithListener::setDestroyListener( const FactoryDestroyListenerInterfacePtr & _listener )
    {
        m_destroyListener = _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    void FactoryWithListener::destroyObject( Factorable * _object )
    {
        STDEX_THREAD_GUARD_CHECK( this, "Factory::destroyObject" );

        if( m_destroyListener != nullptr )
        {
            m_destroyListener->onFactoryDestroyObject( _object );
        }

        this->_destroyObject( _object );

        --m_count;
        intrusive_ptr_dec_ref( this );
    }
    //////////////////////////////////////////////////////////////////////////
}
