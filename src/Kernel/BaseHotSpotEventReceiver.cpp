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
    bool BaseHotSpotEventReceiver::onHotSpotMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotKey( const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotText( const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotAccelerometer( const InputAccelerometerEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButton( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseMove( const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _event );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHotSpotEventReceiver::onHotSpotMouseWheel( const InputMouseWheelEvent & _event )
    {
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
    void BaseHotSpotEventReceiver::onHotSpotMouseButtonBegin( UniqueId _enumerator, bool _isEnd )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _isEnd );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHotSpotEventReceiver::onHotSpotMouseButtonEnd( UniqueId _enumerator, bool _isEnd )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _isEnd );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}