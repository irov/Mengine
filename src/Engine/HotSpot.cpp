#include "HotSpot.h"

#include "Interface/MousePickerSystemInterface.h"
#include "Interface/PlayerInterface.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Arrow.h"

#include "pybind/system.hpp"
#include "pybind/extract.hpp"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpot::HotSpot()
        : m_debugColor( 0x00000000 )
        , m_outward( false )
        , m_global( false )
        , m_picker( nullptr )
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
    bool HotSpot::isMousePickerOver() const
    {
        if( m_picker == nullptr )
        {
            LOGGER_ERROR( "HotSpot::isMousePickerOver %s not activate picker"
                , this->getName().c_str()
            );

            return false;
        }

        bool picked = m_picker->picked;

        return picked;
    }
    //////////////////////////////////////////////////////////////////////////
    MousePickerTrapInterfacePtr HotSpot::getPickerTrap()
    {
        return this;
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
    PickerTrapState * HotSpot::propagatePickerTrapState() const
    {
        return m_picker;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::activatePicker_()
    {
        if( m_picker != nullptr )
        {
            LOGGER_ERROR( "HotSpot::activatePicker_ '%s' alredy activate picker"
                , this->getName().c_str()
            );

            return;
        }

        const MousePickerSystemInterfacePtr & mousePickerSystem = PLAYER_SERVICE()
            ->getMousePickerSystem();

        if( mousePickerSystem == nullptr )
        {
            LOGGER_ERROR( "HotSpot::activatePicker_ '%s' invalid get mouse picker system"
                , this->getName().c_str()
            );

            return;
        }

        m_debugColor = 0xFFFFFFFF;

        m_picker = mousePickerSystem->regTrap( this );

        mousePickerSystem->updateTraps();

        EVENTABLE_METHOD( this, EVENT_ACTIVATE )
            ->onHotSpotActivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::deactivatePicker_()
    {
        if( m_picker == nullptr )
        {
            LOGGER_ERROR( "HotSpot::activatePicker_ '%s' alredy deactivate picker"
                , this->getName().c_str()
            );

            return;
        }

        const MousePickerSystemInterfacePtr & mousePickerSystem = PLAYER_SERVICE()
            ->getMousePickerSystem();

        if( mousePickerSystem == nullptr )
        {
            LOGGER_ERROR( "HotSpot::activatePicker_ '%s' invalid get mouse picker system"
                , this->getName().c_str()
            );

            return;
        }

        PickerTrapState * picker = m_picker;
        m_picker = nullptr;

        mousePickerSystem->unregTrap( picker );

        mousePickerSystem->updateTraps();

        m_debugColor = 0x00000000;

        EVENTABLE_METHOD( this, EVENT_DEACTIVATE )
            ->onHotSpotDeactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::onHandleMouseLeave()
    {
        m_debugColor = 0xFFFFFFFF;

        EVENTABLE_METHOD( this, EVENT_MOUSE_LEAVE )
            ->onHotSpotMouseLeave();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::onHandleMouseOverDestroy()
    {
        m_debugColor = 0xFFFFFFFF;

        EVENTABLE_METHOD( this, EVENT_MOUSE_OVER_DESTROY )
            ->onHotSpotMouseOverDestroy();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::onHandleMouseEnter( float _x, float _y )
    {
        m_debugColor = 0xFFFF0000;

        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_ENTER, m_defaultHandle )
            ->onHotSpotMouseEnter( _x, _y );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleKeyEvent( const InputKeyEvent & _event )
    {
        //bool handle = m_defaultHandle;

        bool handle = EVENTABLE_METHODR( this, EVENT_KEY, m_defaultHandle )
            ->onHotSpotKey( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleTextEvent( const InputTextEvent & _event )
    {
        //bool handle = m_defaultHandle;

        bool handle = EVENTABLE_METHODR( this, EVENT_TEXT, m_defaultHandle )
            ->onHotSpotText( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        //bool handle = m_defaultHandle;

        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_BUTTON, m_defaultHandle )
            ->onHotSpotMouseButton( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool HotSpot::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_BUTTON_BEGIN, false )
            ->onHotSpotMouseButtonBegin( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_BUTTON_END, false )
            ->onHotSpotMouseButtonEnd( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_MOVE, m_defaultHandle )
            ->onHotSpotMouseMove( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( this, EVENT_MOUSE_WHEEL, m_defaultHandle )
            ->onHotSpotMouseWheel( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_setLocalHide( bool _value )
    {
        Node::_setLocalHide( _value );

        if( _value == true )
        {
            if( this->isAfterActive() == true && this->isFreeze() == false )
            {
                this->deactivatePicker_();
            }
        }
        else
        {
            if( this->isAfterActive() == true && this->isFreeze() == false )
            {
                this->activatePicker_();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_freeze( bool _value )
    {
        Node::_freeze( _value );

        if( _value == true )
        {
            if( this->isAfterActive() == true && this->getLocalHide() == false )
            {
                this->deactivatePicker_();
            }
        }
        else
        {
            if( this->isAfterActive() == true && this->getLocalHide() == false )
            {
                this->activatePicker_();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        if( m_picker == nullptr )
        {
            return;
        }

        const MousePickerSystemInterfacePtr & mousePickerSystem = PLAYER_SERVICE()
            ->getMousePickerSystem();

        if( mousePickerSystem == nullptr )
        {
            LOGGER_ERROR( "HotSpot::activatePicker_ '%s' invalid get mouse picker system"
                , this->getName().c_str()
            );

            return;
        }

        mousePickerSystem->invalidateTraps();
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
    void HotSpot::_afterActivate()
    {
        Node::_afterActivate();

        if( this->getLocalHide() == false && this->isFreeze() == false )
        {
            this->activatePicker_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_deactivate()
    {
        if( this->getLocalHide() == false && this->isFreeze() == false )
        {
            this->deactivatePicker_();
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow )
    {
        if( this->isActivate() == false )
        {
            return false;
        }

        EArrowType arrowType = _arrow->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                bool result = this->testPoint( _camera, _viewport, _contentResolution, _point );

                return result;
            }break;
        case EAT_RADIUS:
            {
                float radius = _arrow->getRadius();

                mt::vec2f rxy;
                Helper::worldToScreenDelta( _camera, _viewport, _contentResolution, mt::vec2f( radius, radius ), rxy );

                bool result = this->testRadius( _camera, _viewport, _contentResolution, _point, rxy.x, rxy.y );

                return result;
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = _arrow->getPolygon();

                //m_result = m_hotspot->testPolygon( m_camera, m_viewport, m_gameViewport, m_point, polygon );
                bool result = this->testPolygon( _camera, _viewport, _contentResolution, _point, polygon );

                return result;
            }break;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ScriptablePtr HotSpot::propagatePickerScriptable()
    {
        return this;
    }
}
