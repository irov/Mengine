#include "FactoryWithListener.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FactoryWithListener::FactoryWithListener()
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

        FactoryWithMutex::destroyObject( _object );
    }
    //////////////////////////////////////////////////////////////////////////
}
