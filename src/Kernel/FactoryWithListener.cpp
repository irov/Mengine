#include "FactoryWithListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithListener::FactoryWithListener( const Char * _name )
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
        if( m_destroyListener != nullptr )
        {
            m_destroyListener->onFactoryDestroyObject( _object );
        }

        Factory::destroyObject( _object );
    }
    //////////////////////////////////////////////////////////////////////////
}
