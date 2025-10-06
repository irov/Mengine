#include "HotSpot.h"

#include "Interface/PickerServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"

#include "Kernel/RenderCameraHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/AssertionContainer.h"

#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpot::HotSpot()
        : m_outward( false )
        , m_global( false )
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
        this->setPickerExclusive( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::getExclusive() const
    {
        bool exclusive = this->isPickerExclusive();

        return exclusive;
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
    void HotSpot::addPickerInputHandler( const PickerInputHandlerInterfacePtr & _inputHandler, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        PickerInputHandlerDesc desc;
        desc.handler = _inputHandler;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif
        
        m_pickerInputHandlers.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::removePickerInputHandler( const PickerInputHandlerInterfacePtr & _inputHandler )
    {
        VectorPickerInputHandlers::iterator it_found = StdAlgorithm::find_if( m_pickerInputHandlers.begin(), m_pickerInputHandlers.end(), [_inputHandler]( const PickerInputHandlerDesc & _desc )
        {
            return _desc.handler == _inputHandler;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_pickerInputHandlers.end(), "HotSpot '%s' input handler not found"
            , this->getName().c_str()
        );

        m_pickerInputHandlers.erase( it_found );
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
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
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
    PickerInputHandlerInterface * HotSpot::getPickerInputHandler()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_ENTER, m_defaultHandle )
            ->onHotSpotMouseEnter( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseEnter( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_HOTSPOT_MOUSE_LEAVE )
            ->onHotSpotMouseLeave( _context, _event );
        
        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            handler->handleMouseLeave( _context, _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_KEY, m_defaultHandle )
            ->onHotSpotKey( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleKeyEvent( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleTextEvent( const RenderContext * _context, const InputTextEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_TEXT, m_defaultHandle )
            ->onHotSpotText( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleTextEvent( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON, m_defaultHandle )
            ->onHotSpotMouseButton( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseButtonEvent( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN, false )
            ->onHotSpotMouseButtonBegin( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseButtonEventBegin( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_BUTTON_END, false )
            ->onHotSpotMouseButtonEnd( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseButtonEventEnd( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_MOVE, m_defaultHandle )
            ->onHotSpotMouseMove( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseMove( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        bool handle = EVENTABLE_METHODR( EVENT_HOTSPOT_MOUSE_WHEEL, m_defaultHandle )
            ->onHotSpotMouseWheel( _context, _event );

        if( handle == true )
        {
            return true;
        }

        for( const PickerInputHandlerDesc & desc : m_pickerInputHandlers )
        {
            const PickerInputHandlerInterfacePtr & handler = desc.handler;

            if( handler->handleMouseWheel( _context, _event ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::_activate()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        this->setPickerPicked( false );
        this->setPickerPressed( false );
        this->setPickerHandle( false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_deactivate()
    {
        this->setPickerPressed( false );
        this->setPickerHandle( false );

        if( this->isPickerPicked() == true )
        {
            this->setPickerPicked( false );

            EVENTABLE_METHOD( EVENT_HOTSPOT_MOUSE_OVER_DESTROY )
                ->onHotSpotMouseOverDestroy();
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpot::_dispose()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_pickerInputHandlers );

        m_pickerInputHandlers.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpot::pick( const mt::vec2f & _point, const RenderContext * _context ) const
    {
        EArrowType arrowType = ARROW_SERVICE()
            ->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                bool result = this->testPoint( _context, _point );

                return result;
            }break;
        case EAT_RADIUS:
            {
                float radius = ARROW_SERVICE()
                    ->getArrowRadius();

                bool result = this->testRadius( _context, _point, radius );

                return result;
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = ARROW_SERVICE()
                    ->getArrowPolygon();

                bool result = this->testPolygon( _context, _point, polygon );

                return result;
            }break;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
