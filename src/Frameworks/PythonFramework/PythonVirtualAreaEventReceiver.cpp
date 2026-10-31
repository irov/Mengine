#include "PythonVirtualAreaEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonVirtualAreaEventReceiver::PythonVirtualAreaEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonVirtualAreaEventReceiver::~PythonVirtualAreaEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaDrag( const mt::vec2f & _position, const mt::vec2f & _percentage )
    {
        m_cb.call( _position, _percentage );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaDragStart( const mt::vec2f & _position )
    {
        m_cb.call( _position );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaDragMove( const mt::vec2f & _position, const mt::vec2f & _velocity )
    {
        m_cb.call( _position, _velocity );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaDragEnd( const mt::vec2f & _position, const mt::vec2f & _velocity )
    {
        m_cb.call( _position, _velocity );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaScale( float _scaleFactor )
    {
        m_cb.call( _scaleFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonVirtualAreaEventReceiver::onVirtualAreaTouch( uint32_t _touchCount )
    {
        m_cb.call( _touchCount );
    }
    //////////////////////////////////////////////////////////////////////////
}
