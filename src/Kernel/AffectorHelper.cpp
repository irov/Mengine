#include "Kernel/AffectorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseAffector::BaseAffector()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseAffector::~BaseAffector()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffector::setCallback( const AffectorCallbackInterfacePtr & _cb )
    {
        m_cb = _cb;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorCallbackInterfacePtr & BaseAffector::getCallback() const
    {
        return m_cb;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffector::complete()
    {
        this->end_( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAffector::end_( bool _isEnd )
    {
        if( m_cb != nullptr )
        {
            uint32_t id = this->getId();

            m_cb->onAffectorEnd( id, _isEnd );
        }
    }
}
