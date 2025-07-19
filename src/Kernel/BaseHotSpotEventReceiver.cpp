#include "BaseHotSpotEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotActivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotDeactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotKey( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotText( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButtonBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButtonEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotMouseOverDestroy()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}