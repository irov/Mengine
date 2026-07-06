#include "VirtualAreaGlobalInputHandler.h"

#include "VirtualArea.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaGlobalInputHandler::VirtualAreaGlobalInputHandler( VirtualArea * _area )
        : m_area( _area )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaGlobalInputHandler::~VirtualAreaGlobalInputHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleKeyEvent( const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleTextEvent( const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_area == nullptr )
        {
            return false;
        }

        if( _event.isDown == false )
        {
            m_area->endTouch_( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_area == nullptr )
        {
            return false;
        }

        m_area->endTouch_( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        if( m_area == nullptr )
        {
            return false;
        }

        m_area->moveTouch_( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaGlobalInputHandler::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaGlobalInputHandler::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        if( m_area == nullptr )
        {
            return;
        }

        m_area->releaseTouch_( _event.touchId );
    }
    //////////////////////////////////////////////////////////////////////////
}
