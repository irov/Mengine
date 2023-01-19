#include "HotSpot.h"

#include "Interface/PickerServiceInterface.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Arrow.h"
#include "Kernel/EventableHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpot::HotSpot()
        : m_outward( false )
        , m_global( false )
        , m_exclusive( false )
        , m_defaultHandle( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpot::~HotSpot()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setOutward( bool _value )
    {
        m_outward = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getOutward() const
    {
        return m_outward;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setDefaultHandle( bool _handle )
    {
        m_defaultHandle = _handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getDefaultHandle() const
    {
        return m_defaultHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setGlobal( bool _value )
    {
        m_global = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getGlobal() const
    {
        return m_global;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::setExclusive( bool _value )
    {
        m_exclusive = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getExclusive() const
    {
        return m_exclusive;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::isMousePickerOver() const
    {
        if( this->isPickerEnable() == false )
        {
            return false;
        }

        bool picked = this->isPickerPicked();

        return picked;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::activatePicker_()
    {
        this->setPickerPicked( false );
        this->setPickerPressed( false );
        this->setPickerHandle( false );
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::deactivatePicker_()
    {
        this->setPickerPressed( false );
        this->setPickerHandle( false );

        if( this->isPickerPicked() == true )
        {
            this->setPickerPicked( false );

            EVENTABLE_METHOD( EVENT_HOTSPOT_MOUSE_OVER_DESTROY )
                ->onHotSpotMouseOverDestroy();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_setPickerEnable( bool _enable )
    {
        if( _enable == true )
        {
            EVENTABLE_METHOD( EVENT_HOTSPOT_ACTIVATE )
                ->onHotSpotActivate();
        }
        else
        {
            EVENTABLE_METHOD( EVENT_HOTSPOT_DEACTIVATE )
                ->onHotSpotDeactivate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_invalidateWorldMatrix() const
    {
        if( this->isPickerEnable() == false )
        {
            return;
        }

        PICKER_SERVICE()
            ->invalidateTraps();
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_USE_SCRIPT_SERVICE
    //////////////////////////////////////////////////////////////////////////
    Scriptable * HotSpot::getPickerScriptable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    Eventable * HotSpot::getPickerEventable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    InputHandlerInterface * HotSpot::getPickerInputHandler()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_ENTER, m_defaultHandle )
            ->onHotSpotMouseEnter( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_HOTSPOT_MOUSE_LEAVE )
            ->onHotSpotMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleKeyEvent( const InputKeyEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_KEY, m_defaultHandle )
            ->onHotSpotKey( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleTextEvent( const InputTextEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_TEXT, m_defaultHandle )
            ->onHotSpotText( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON, m_defaultHandle )
            ->onHotSpotMouseButton( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN, false )
            ->onHotSpotMouseButtonBegin( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON_END, false )
            ->onHotSpotMouseButtonEnd( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_MOVE, m_defaultHandle )
            ->onHotSpotMouseMove( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_WHEEL, m_defaultHandle )
            ->onHotSpotMouseWheel( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_freeze( bool _value )
    {
        Node::_freeze( _value );

        if( _value == true )
        {
            if( this->isAfterActive() == true )
            {
                this->deactivatePicker_();
            }
        }
        else
        {
            if( this->isAfterActive() == true )
            {
                this->activatePicker_();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::_afterActivate()
    {
        if( this->isFreeze() == false )
        {
            this->activatePicker_();
        }

        if( Node::_afterActivate() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_deactivate()
    {
        if( this->isFreeze() == false )
        {
            this->deactivatePicker_();
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::pick( const mt::vec2f & _point, const RenderContext * _context, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const
    {
        EArrowType arrowType = _arrow->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                bool result = this->testPoint( _context, _contentResolution, _point );

                return result;
            }break;
        case EAT_RADIUS:
            {
                float radius = _arrow->getRadius();

                bool result = this->testRadius( _context, _contentResolution, _point, radius );

                return result;
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = _arrow->getPolygon();

                bool result = this->testPolygon( _context, _contentResolution, _point, polygon );

                return result;
            }break;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
