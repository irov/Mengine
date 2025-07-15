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

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotText( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );

        return m_cb.call( _context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseOverDestroy()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
}