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
    bool PythonHotSpotEventReceiver::onHotSpotMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.x, _event.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.x, _event.y, _event.code, _event.isDown, _event.isRepeat );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotText( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );

        WChar symbol = _event.text[0];

        return m_cb.call( _event.x, _event.y, symbol );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _event.touchId, _event.x, _event.y, _event.dx, _event.dy, _event.pressure );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );

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