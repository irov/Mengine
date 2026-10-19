#include "PythonHotSpotEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static pybind::object makeRenderContext( const pybind::object & _cb, const RenderContext * _context )
        {
            pybind::kernel_interface * kernel = _cb.kernel();

            // RenderContext binds as a struct, which Python takes by value: a
            // pointer to it has no cast.
            if( _context == nullptr )
            {
                return pybind::make_none_t( kernel );
            }

            return pybind::make_object_t( kernel, *_context );
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotText( const RenderContext * _context, const InputTextEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonHotSpotEventReceiver::onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        pybind::object context = Detail::makeRenderContext( m_cb, _context );

        return m_cb.call( context, _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonHotSpotEventReceiver::onHotSpotMouseOverDestroy()
    {
        m_cb.call();
    }
    //////////////////////////////////////////////////////////////////////////
}