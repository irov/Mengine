#include "PythonHotSpotEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonHotSpotEventReceiver::PythonHotSpotEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonHotSpotEventReceiver::~PythonHotSpotEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotActivate()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotDeactivate()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseEnter( const InputMouseEnterEvent & _event )
    {
        return m_cb.call( _event.x, _event.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotKey( const InputKeyEvent & _event )
    {
        return m_cb.call( _event.x, _event.y, _event.code, _event.isDown, _event.isRepeat );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotText( const InputTextEvent & _event )
    {
        WChar symbol = _event.text[0];

        return m_cb.call( _event.x, _event.y, symbol );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotAccelerometer( const InputAccelerometerEvent & _event )
    {
        return m_cb.call( _event.dx, _event.dy, _event.dz );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButton( const InputMouseButtonEvent & _event )
    {
        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event )
    {
        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event )
    {
        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseMove( const InputMouseMoveEvent & _event )
    {
        return m_cb.call( _event.touchId, _event.x, _event.y, _event.dx, _event.dy, _event.pressure );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseWheel( const InputMouseWheelEvent & _event )
    {
        return m_cb.call( _event.x, _event.y, _event.wheel, _event.scroll );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseOverDestroy()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseButtonBegin( uint32_t _playId, bool _isEnd )
    {
        m_cb.call( _playId, _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseButtonEnd( uint32_t _playId, bool _isEnd )
    {
        m_cb.call( _playId, _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
}