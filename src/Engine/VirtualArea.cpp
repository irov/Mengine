#include "VirtualArea.h"

#include "VirtualAreaGlobalInputHandler.h"

#include "Interface/ArrowServiceInterface.h"
#include "Interface/GlobalInputHandlerInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderResolutionInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"

#include "math/utils.h"

namespace Mengine
{
    namespace
    {
        constexpr float VIRTUAL_AREA_DEFAULT_MAX_SCALE_FACTOR = 24.f;
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualArea::VirtualArea()
        : m_viewport( 0.f, 0.f, 0.f, 0.f )
        , m_bounds( 0.f, 0.f, 0.f, 0.f )
        , m_localBounds( 0.f, 0.f, 0.f, 0.f )
        , m_contentSize( 0.f, 0.f, 2736.f, 1536.f )
        , m_anchor( 0.f, 0.f )
        , m_position( 0.f, 0.f )
        , m_velocity( 0.f, 0.f )
        , m_returnStart( 0.f, 0.f )
        , m_returnTarget( 0.f, 0.f )
        , m_scaleFactor( 1.f )
        , m_maxScaleFactor( VIRTUAL_AREA_DEFAULT_MAX_SCALE_FACTOR )
        , m_wheelScaleFactor( 0.375f )
        , m_friction( 0.5f )
        , m_rigidity( 0.5f )
        , m_dragStartThreshold( 50.f )
        , m_lastPinchDistance( 0.f )
        , m_returnTime( 0.f )
        , m_returnDuration( 0.f )
        , m_draggingMode( EVADM_FREE )
        , m_draggingModeName( STRINGIZE_STRING_LOCAL( "free" ) )
        , m_globalHandlerId( INVALID_UNIQUE_ID )
        , m_hasViewport( false )
        , m_enableScale( true )
        , m_allowOutOfBounds( true )
        , m_disableDragIfInvalid( true )
        , m_defaultHandle( true )
        , m_frozen( false )
        , m_dragging( false )
        , m_pinchActive( false )
        , m_returnActive( false )
    {
        m_touches.resize( MENGINE_INPUT_MAX_TOUCH );

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            TouchDesc & touch = m_touches[index];

            touch.active = false;
            touch.pressed = false;
            touch.dragging = false;
            touch.touchId = (ETouchCode)index;
            touch.startScreen = mt::vec2f( 0.f, 0.f );
            touch.screen = mt::vec2f( 0.f, 0.f );
            touch.previousScreen = mt::vec2f( 0.f, 0.f );
            touch.startWorld = mt::vec2f( 0.f, 0.f );
            touch.world = mt::vec2f( 0.f, 0.f );
            touch.previousWorld = mt::vec2f( 0.f, 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualArea::~VirtualArea()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setViewport( const Viewport & _viewport )
    {
        this->stopElasticReturn_();

        m_hasViewport = true;
        m_viewport = _viewport;
        m_bounds = _viewport;

        m_scaleFactor = 1.f;

        this->updateLocalBounds_();
        this->updateRenderContextNodes_();
        this->setPositionInternal_( m_localBounds.begin, true );
        this->validatePosition_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & VirtualArea::getViewport() const
    {
        return m_viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::hasViewport() const
    {
        return m_hasViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setViewportFromHotSpot( const HotSpotPolygonPtr & _hotSpot )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _hotSpot, "virtual area '%s' invalid hotspot"
            , this->getName().c_str()
        );

        const Polygon & polygon = _hotSpot->getPolygon();

        mt::box2f box;
        polygon.to_box2f( &box );

        this->setViewport( Viewport( box.minimum, box.maximum ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setContentSize( float _left, float _top, float _right, float _bottom )
    {
        this->stopElasticReturn_();

        m_contentSize = mt::vec4f( _left, _top, _right, _bottom );

        this->validatePosition_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & VirtualArea::getContentSize() const
    {
        return m_contentSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::addContentNode( const NodePtr & _node, bool _useAsAnchor )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _node, "virtual area '%s' invalid content node"
            , this->getName().c_str()
        );

        if( _useAsAnchor == true )
        {
            const TransformationInterface * transformation = _node->getTransformation();
            const mt::vec3f & position = transformation->getWorldPosition();
            this->setAnchor( mt::vec2f( position.x, position.y ) );
        }

        this->addChild( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setPosition( const mt::vec2f & _position )
    {
        this->stopElasticReturn_();

        m_velocity = mt::vec2f( 0.f, 0.f );

        this->setPositionInternal_( _position, true );

        if( m_allowOutOfBounds == false )
        {
            this->validatePosition_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & VirtualArea::getPosition() const
    {
        return m_position;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setAnchor( const mt::vec2f & _anchor )
    {
        this->stopElasticReturn_();

        m_anchor = _anchor;

        this->updateLocalBounds_();
        this->setPositionInternal_( m_localBounds.begin, true );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & VirtualArea::getAnchor() const
    {
        return m_anchor;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setPercentage( const mt::vec2f & _percentage )
    {
        this->setPercentageX( _percentage.x );
        this->setPercentageY( _percentage.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setPercentageX( float _percentage )
    {
        this->stopElasticReturn_();

        const float contentWidth = mt::abs_f( m_contentSize.z - m_contentSize.x );
        const float gap = m_bounds.getWidth() - contentWidth;

        m_velocity.x = 0.f;

        if( mt::abs_f( gap ) < 0.0001f )
        {
            return;
        }

        mt::vec2f position = m_position;
        position.x = m_localBounds.begin.x + gap * _percentage;

        this->setPositionInternal_( position, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setPercentageY( float _percentage )
    {
        this->stopElasticReturn_();

        const float contentHeight = mt::abs_f( m_contentSize.w - m_contentSize.y );
        const float gap = m_bounds.getHeight() - contentHeight;

        m_velocity.y = 0.f;

        if( mt::abs_f( gap ) < 0.0001f )
        {
            return;
        }

        mt::vec2f position = m_position;
        position.y = m_localBounds.begin.y + gap * _percentage;

        this->setPositionInternal_( position, true );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getPercentage() const
    {
        const float contentWidth = mt::abs_f( m_contentSize.z - m_contentSize.x );
        const float contentHeight = mt::abs_f( m_contentSize.w - m_contentSize.y );

        const float horizontalGap = m_bounds.getWidth() - contentWidth;
        const float verticalGap = m_bounds.getHeight() - contentHeight;

        mt::vec2f percentage( 0.f, 0.f );

        if( mt::abs_f( horizontalGap ) > 0.0001f )
        {
            percentage.x = mt::clamp( -1.f, (m_position.x - m_localBounds.begin.x) / horizontalGap, 1.f );
        }

        if( mt::abs_f( verticalGap ) > 0.0001f )
        {
            percentage.y = mt::clamp( -1.f, (m_position.y - m_localBounds.begin.y) / verticalGap, 1.f );
        }

        return percentage;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::scale( float _factor )
    {
        this->stopElasticReturn_();

        if( m_enableScale == false )
        {
            return;
        }

        if( _factor <= 0.f )
        {
            return;
        }

        const float minScale = 1.f / m_maxScaleFactor;
        float nextScale = m_scaleFactor * _factor;

        if( nextScale > 1.f )
        {
            _factor = 1.f / m_scaleFactor;
            nextScale = 1.f;
        }
        else if( nextScale < minScale )
        {
            _factor = minScale / m_scaleFactor;
            nextScale = minScale;
        }

        mt::vec2f center;
        m_bounds.getCenter( &center );

        const mt::vec2f size = m_bounds.size();
        const mt::vec2f scaledSize = size / _factor;
        const mt::vec2f halfSize = scaledSize * 0.5f;

        m_bounds.setRectangle( center - halfSize, center + halfSize );
        m_scaleFactor = nextScale;

        this->updateLocalBounds_();
        this->updateRenderContextNodes_();

        if( m_allowOutOfBounds == false )
        {
            this->validatePosition_();
        }

        EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_SCALE )
            ->onVirtualAreaScale( m_scaleFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setScale( float _scaleFactor )
    {
        if( m_scaleFactor <= 0.f )
        {
            return;
        }

        this->scale( _scaleFactor / m_scaleFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getScaleFactor() const
    {
        return m_scaleFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setMaxScaleFactor( float _maxScaleFactor )
    {
        m_maxScaleFactor = _maxScaleFactor < 1.f ? 1.f : _maxScaleFactor;

        const float minScale = 1.f / m_maxScaleFactor;

        if( m_scaleFactor < minScale )
        {
            this->setScale( minScale );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getMaxScaleFactor() const
    {
        return m_maxScaleFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setScaleEnable( bool _enable )
    {
        m_enableScale = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::getScaleEnable() const
    {
        return m_enableScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setWheelScaleFactor( float _factor )
    {
        m_wheelScaleFactor = _factor < 0.f ? 0.f : _factor;
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getWheelScaleFactor() const
    {
        return m_wheelScaleFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setDraggingMode( EVirtualAreaDragMode _mode )
    {
        m_draggingMode = _mode;

        switch( m_draggingMode )
        {
        case EVADM_NONE:
            m_draggingModeName = STRINGIZE_STRING_LOCAL( "none" );
            break;
        case EVADM_FREE:
            m_draggingModeName = STRINGIZE_STRING_LOCAL( "free" );
            break;
        case EVADM_HORIZONTAL:
            m_draggingModeName = STRINGIZE_STRING_LOCAL( "horizontal" );
            break;
        case EVADM_VERTICAL:
            m_draggingModeName = STRINGIZE_STRING_LOCAL( "vertical" );
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    EVirtualAreaDragMode VirtualArea::getDraggingMode() const
    {
        return m_draggingMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setDraggingModeName( const ConstString & _mode )
    {
        if( _mode == STRINGIZE_STRING_LOCAL( "none" ) )
        {
            this->setDraggingMode( EVADM_NONE );
        }
        else if( _mode == STRINGIZE_STRING_LOCAL( "horizontal" ) )
        {
            this->setDraggingMode( EVADM_HORIZONTAL );
        }
        else if( _mode == STRINGIZE_STRING_LOCAL( "vertical" ) )
        {
            this->setDraggingMode( EVADM_VERTICAL );
        }
        else
        {
            this->setDraggingMode( EVADM_FREE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & VirtualArea::getDraggingModeName() const
    {
        return m_draggingModeName;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setFriction( float _friction )
    {
        m_friction = mt::clamp( 0.f, _friction, 1.f );
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getFriction() const
    {
        return m_friction;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setRigidity( float _rigidity )
    {
        m_rigidity = mt::clamp( 0.f, _rigidity, 1.f );
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getRigidity() const
    {
        return m_rigidity;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setAllowOutOfBounds( bool _allow )
    {
        if( _allow == false )
        {
            this->stopElasticReturn_();
        }

        m_allowOutOfBounds = _allow;

        if( m_allowOutOfBounds == false )
        {
            this->validatePosition_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::getAllowOutOfBounds() const
    {
        return m_allowOutOfBounds;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setDisableDragIfInvalid( bool _disable )
    {
        m_disableDragIfInvalid = _disable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::getDisableDragIfInvalid() const
    {
        return m_disableDragIfInvalid;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setDragStartThreshold( float _threshold )
    {
        m_dragStartThreshold = _threshold < 0.f ? 0.f : _threshold;
    }
    //////////////////////////////////////////////////////////////////////////
    float VirtualArea::getDragStartThreshold() const
    {
        return m_dragStartThreshold;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setDefaultHandle( bool _handle )
    {
        m_defaultHandle = _handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::getDefaultHandle() const
    {
        return m_defaultHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::freeze( bool _value )
    {
        Node::freeze( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::isFrozen() const
    {
        return m_frozen;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & VirtualArea::getVelocity() const
    {
        return m_velocity;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::isDragging() const
    {
        return m_dragging;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t VirtualArea::getTouchCount() const
    {
        uint32_t count = 0;

        for( const TouchDesc & touch : m_touches )
        {
            if( touch.active == true )
            {
                ++count;
            }
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getViewportSize() const
    {
        return m_viewport.size();
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getContentSizeValue() const
    {
        return mt::vec2f( mt::abs_f( m_contentSize.z - m_contentSize.x ), mt::abs_f( m_contentSize.w - m_contentSize.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraOrthogonalPtr & VirtualArea::getRenderCameraNode() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportPtr & VirtualArea::getRenderViewportNode() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorPtr & VirtualArea::getRenderScissorNode() const
    {
        return m_renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::_activate()
    {
        this->ensureRenderContextNodes_();
        this->updateRenderContextNodes_();
        this->updateCameraPosition_();

        if( m_globalHandler == nullptr )
        {
            VirtualAreaGlobalInputHandlerPtr handler = Helper::makeFactorableUnique<VirtualAreaGlobalInputHandler>( MENGINE_DOCUMENT_FACTORABLE, this );

            m_globalHandler = handler;
        }

        const GlobalInputHandlerInterfacePtr & globalInputHandler = PLAYER_SERVICE()
            ->getGlobalInputHandler();

        m_globalHandlerId = globalInputHandler->addGlobalHandler( m_globalHandler, MENGINE_DOCUMENT_FACTORABLE );
        globalInputHandler->enableGlobalHandler( m_globalHandlerId, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::_deactivate()
    {
        this->stopDrag_();

        if( m_globalHandlerId != INVALID_UNIQUE_ID )
        {
            const GlobalInputHandlerInterfacePtr & globalInputHandler = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            globalInputHandler->removeGlobalHandler( m_globalHandlerId );
            m_globalHandlerId = INVALID_UNIQUE_ID;
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::_dispose()
    {
        m_renderCamera = nullptr;
        m_renderViewport = nullptr;
        m_renderScissor = nullptr;
        m_globalHandler = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::_freeze( bool _value )
    {
        m_frozen = _value;

        if( _value == true )
        {
            this->stopDrag_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::update( const UpdateContext * _context )
    {
        if( m_frozen == true || m_dragging == true )
        {
            return;
        }

        const float dt = _context->time;

        if( m_returnActive == true )
        {
            if( this->updateElasticReturn_( dt ) == true )
            {
                return;
            }
        }

        mt::vec2f offset = this->getBoundsOffset_( m_position );

        if( this->isZero_( offset ) == false )
        {
            if( m_allowOutOfBounds == false )
            {
                mt::vec2f position = m_position + offset;

                if( mt::abs_f( offset.x ) > 0.0001f )
                {
                    m_velocity.x = 0.f;
                }

                if( mt::abs_f( offset.y ) > 0.0001f )
                {
                    m_velocity.y = 0.f;
                }

                this->setPositionInternal_( position, true );

                if( this->isZero_( m_velocity ) == true )
                {
                    return;
                }
            }
            else
            {
                if( this->startElasticReturn_( offset ) == true )
                {
                    this->updateElasticReturn_( dt );
                }

                return;
            }
        }

        if( this->isZero_( m_velocity ) == true )
        {
            return;
        }

        const float friction = 0.0005f + m_friction * 0.015f;
        const float velocityLength = mt::length_v2( m_velocity );

        if( velocityLength <= 0.01f )
        {
            m_velocity = mt::vec2f( 0.f, 0.f );

            return;
        }

        const float newLength = velocityLength - friction * dt;

        if( newLength <= 0.f )
        {
            m_velocity = mt::vec2f( 0.f, 0.f );

            return;
        }

        const float k = newLength / velocityLength;
        m_velocity *= k;

        mt::vec2f nextPosition = m_position + m_velocity * dt;

        if( m_allowOutOfBounds == false )
        {
            mt::vec2f clampedPosition = nextPosition;

            if( this->clampPositionToBounds_( &clampedPosition ) == true )
            {
                if( mt::abs_f( clampedPosition.x - nextPosition.x ) > 0.0001f )
                {
                    m_velocity.x = 0.f;
                }

                if( mt::abs_f( clampedPosition.y - nextPosition.y ) > 0.0001f )
                {
                    m_velocity.y = 0.f;
                }

                nextPosition = clampedPosition;
            }
        }

        this->setPositionInternal_( nextPosition, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::pick( const mt::vec2f & _point, const RenderContext * _context ) const
    {
        if( m_hasViewport == false || _context == nullptr || _context->resolution == nullptr )
        {
            return false;
        }

        const RenderResolutionInterface * resolution = _context->resolution;

        Viewport viewport = m_viewport;

        if( m_renderViewport != nullptr )
        {
            viewport = m_renderViewport->getViewportWM();
        }

        mt::vec2f screenBegin;
        mt::vec2f screenEnd;
        resolution->fromContentToScreenPosition( viewport.begin, &screenBegin );
        resolution->fromContentToScreenPosition( viewport.end, &screenEnd );

        const float left = screenBegin.x < screenEnd.x ? screenBegin.x : screenEnd.x;
        const float right = screenBegin.x < screenEnd.x ? screenEnd.x : screenBegin.x;
        const float top = screenBegin.y < screenEnd.y ? screenBegin.y : screenEnd.y;
        const float bottom = screenBegin.y < screenEnd.y ? screenEnd.y : screenBegin.y;

        if( _point.x < left || _point.x > right )
        {
            return false;
        }

        if( _point.y < top || _point.y > bottom )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    //////////////////////////////////////////////////////////////////////////
    Scriptable * VirtualArea::getPickerScriptable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    Eventable * VirtualArea::getPickerEventable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    PickerInputHandlerInterface * VirtualArea::getPickerInputHandler()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleTextEvent( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        if( m_frozen == true )
        {
            return false;
        }

        if( _event.isDown == true )
        {
            this->beginTouch_( _context, _event );
        }
        else
        {
            this->endTouch_( _event );
        }

        return m_defaultHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );

        if( m_frozen == true || m_enableScale == false )
        {
            return false;
        }

        if( _event.scroll > 0 )
        {
            this->scale( 1.f - m_wheelScaleFactor );
        }
        else
        {
            this->scale( 1.f + m_wheelScaleFactor );
        }

        return m_defaultHandle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderResolutionInterfacePtr & VirtualArea::getPickerResolution() const
    {
        const RenderResolutionInterfacePtr & resolution = this->getRenderResolution();

        return resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & VirtualArea::getPickerViewport() const
    {
        const RenderViewportInterfacePtr & viewport = this->getRenderViewport();

        return viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & VirtualArea::getPickerCamera() const
    {
        const RenderCameraInterfacePtr & camera = this->getRenderCamera();

        return camera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & VirtualArea::getPickerScissor() const
    {
        const RenderScissorInterfacePtr & scissor = this->getRenderScissor();

        return scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & VirtualArea::getPickerTransformation() const
    {
        const RenderTransformationInterfacePtr & transformation = this->getRenderTransformation();

        return transformation;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & VirtualArea::getPickerTarget() const
    {
        const RenderTargetInterfacePtr & target = this->getRenderTarget();

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::ensureRenderContextNodes_()
    {
        if( m_renderCamera == nullptr )
        {
            RenderCameraOrthogonalPtr renderCamera = Helper::generateNodeFactorable<RenderCameraOrthogonal>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderCamera, "virtual area '%s' invalid create RenderCameraOrthogonal"
                , this->getName().c_str()
            );

            renderCamera->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaRenderCameraOrthogonal" ) );
            renderCamera->setProxyViewMatrix( true );
            renderCamera->setFixedOrthogonalViewport( true );

            this->addChild( renderCamera );

            m_renderCamera = renderCamera;
        }

        if( m_renderViewport == nullptr )
        {
            RenderViewportPtr renderViewport = Helper::generateNodeFactorable<RenderViewport>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderViewport, "virtual area '%s' invalid create RenderViewport"
                , this->getName().c_str()
            );

            renderViewport->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaRenderViewport" ) );

            this->addChild( renderViewport );

            m_renderViewport = renderViewport;
        }

        if( m_renderScissor == nullptr )
        {
            RenderScissorPtr renderScissor = Helper::generateNodeFactorable<RenderScissor>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderScissor, "virtual area '%s' invalid create RenderScissor"
                , this->getName().c_str()
            );

            renderScissor->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaRenderScissor" ) );

            this->addChild( renderScissor );

            m_renderScissor = renderScissor;
        }

        RenderInterface * render = this->getRender();
        render->setRenderCamera( m_renderCamera );
        render->setRenderViewport( m_renderViewport );
        render->setRenderScissor( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::updateRenderContextNodes_()
    {
        this->ensureRenderContextNodes_();

        m_renderCamera->setProxyViewMatrix( true );
        m_renderCamera->setFixedOrthogonalViewport( true );
        m_renderCamera->setOrthogonalViewport( m_bounds );

        m_renderViewport->setViewport( m_viewport );
        this->updateRenderScissor_();
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::updateCameraPosition_()
    {
        if( m_renderCamera == nullptr )
        {
            return;
        }

        TransformationInterface * transformation = m_renderCamera->getTransformation();
        transformation->setLocalPosition( mt::vec3f( -m_position.x, -m_position.y, 0.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::updateRenderScissor_()
    {
        if( m_renderScissor == nullptr )
        {
            return;
        }

        Viewport scissorViewport( m_bounds.begin - m_position, m_bounds.end - m_position );
        m_renderScissor->setScissorViewport( scissorViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::updateScaleViewport_()
    {
        if( m_renderCamera != nullptr )
        {
            m_renderCamera->setOrthogonalViewport( m_bounds );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::updateLocalBounds_()
    {
        m_localBounds.setRectangle( m_bounds.begin - m_anchor, m_bounds.end - m_anchor );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::validatePosition_()
    {
        if( m_allowOutOfBounds == true )
        {
            return;
        }

        mt::vec2f position = m_position;

        if( this->clampPositionToBounds_( &position ) == true )
        {
            this->setPositionInternal_( position, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::stopDrag_()
    {
        const uint32_t oldTouchCount = this->getTouchCount();

        for( TouchDesc & touch : m_touches )
        {
            touch.active = false;
            touch.pressed = false;
            touch.dragging = false;
        }

        m_dragging = false;
        m_pinchActive = false;
        m_lastPinchDistance = 0.f;
        m_velocity = mt::vec2f( 0.f, 0.f );
        this->stopElasticReturn_();

        this->setGlobalHandlerEnable_( false );

        if( oldTouchCount != 0 )
        {
            this->notifyTouch_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::beginTouch_( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        if( _context == nullptr )
        {
            return false;
        }

        TouchDesc * touch = this->getTouch_( _event.touchId );

        if( touch == nullptr )
        {
            return false;
        }

        touch->active = true;
        touch->pressed = true;
        touch->dragging = false;
        touch->context = *_context;
        touch->startScreen = _event.position.screen;
        touch->screen = _event.position.screen;
        touch->previousScreen = _event.position.screen;
        touch->startWorld = _event.position.world;
        touch->world = _event.position.world;
        touch->previousWorld = _event.position.world;

        this->stopElasticReturn_();

        m_velocity = mt::vec2f( 0.f, 0.f );
        m_pinchActive = false;
        m_lastPinchDistance = 0.f;

        this->setGlobalHandlerEnable_( m_frozen == false );
        this->notifyTouch_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::moveTouch_( const InputMouseMoveEvent & _event )
    {
        if( m_frozen == true )
        {
            return false;
        }

        TouchDesc * touch = this->getTouch_( _event.touchId );

        if( touch == nullptr || touch->active == false )
        {
            return false;
        }

        touch->previousWorld = touch->world;
        touch->previousScreen = touch->screen;
        touch->screen = _event.position.screen;

        mt::vec2f worldPosition;
        ARROW_SERVICE()
            ->calcMouseWorldPosition( &touch->context, _event.position.screen, &worldPosition );

        touch->world = worldPosition;

        if( m_enableScale == true && this->getTouchCount() == 2 )
        {
            return this->updatePinch_();
        }

        if( this->getTouchCount() != 1 )
        {
            return false;
        }

        if( m_dragging == false )
        {
            mt::vec2f startScreenDelta = touch->screen - touch->startScreen;
            mt::vec2f startContentDelta = this->getScreenContentDelta_( startScreenDelta, &touch->context );
            mt::vec2f testDelta = this->getDragModeDelta_( startContentDelta );

            if( mt::length_v2( testDelta ) < m_dragStartThreshold )
            {
                return false;
            }

            m_dragging = true;
            touch->dragging = true;

            EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_DRAG_START )
                ->onVirtualAreaDragStart( m_position );
        }

        mt::vec2f worldDelta = this->getScreenWorldDelta_( _event.screenDelta, &touch->context );
        mt::vec2f delta = this->getModeDelta_( worldDelta );

        if( this->isZero_( delta ) == true )
        {
            return false;
        }

        m_velocity = delta;

        if( m_allowOutOfBounds == false )
        {
            mt::vec2f nextPosition = m_position + m_velocity;

            this->clampPositionToBounds_( &nextPosition );
            m_velocity = nextPosition - m_position;

            if( this->isZero_( m_velocity ) == true )
            {
                return false;
            }
        }
        else
        {
            mt::vec2f offset = this->getBoundsOffset_( m_position );
            float dot = m_velocity.x * offset.x + m_velocity.y * offset.y;

            if( dot < 0.f )
            {
                const float limit = 100.f;
                const float kx = (limit - mt::abs_f( offset.x )) / limit;
                const float ky = (limit - mt::abs_f( offset.y )) / limit;

                m_velocity.x *= mt::clamp( 0.f, kx, 1.f );
                m_velocity.y *= mt::clamp( 0.f, ky, 1.f );
            }
        }

        this->setPositionInternal_( m_position + m_velocity, true );

        EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_DRAG_MOVE )
            ->onVirtualAreaDragMove( m_position, m_velocity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::endTouch_( const InputMouseButtonEvent & _event )
    {
        if( _event.isDown == true )
        {
            return false;
        }

        return this->releaseTouch_( _event.touchId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::releaseTouch_( ETouchCode _touchId )
    {
        TouchDesc * touch = this->getTouch_( _touchId );

        if( touch == nullptr || touch->active == false )
        {
            return false;
        }

        touch->active = false;
        touch->pressed = false;
        touch->dragging = false;

        if( m_dragging == true )
        {
            const float velocityLimit = 150.f;
            const float velocityLength = mt::length_v2( m_velocity );

            if( velocityLength > velocityLimit )
            {
                m_velocity *= velocityLimit / velocityLength;
            }

            m_velocity *= 0.05f;

            EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_DRAG_END )
                ->onVirtualAreaDragEnd( m_position, m_velocity );
        }

        m_dragging = false;
        m_pinchActive = false;
        m_lastPinchDistance = 0.f;

        if( this->getTouchCount() == 0 && m_allowOutOfBounds == true )
        {
            mt::vec2f offset = this->getBoundsOffset_( m_position );
            this->startElasticReturn_( offset );
        }

        if( this->getTouchCount() == 0 )
        {
            this->setGlobalHandlerEnable_( false );
        }

        this->notifyTouch_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::updatePinch_()
    {
        const TouchDesc * touch0 = nullptr;
        const TouchDesc * touch1 = nullptr;

        for( const TouchDesc & touch : m_touches )
        {
            if( touch.active == false )
            {
                continue;
            }

            if( touch0 == nullptr )
            {
                touch0 = &touch;
            }
            else
            {
                touch1 = &touch;
                break;
            }
        }

        if( touch0 == nullptr || touch1 == nullptr )
        {
            return false;
        }

        const float currentDistance = mt::length_v2( touch0->world - touch1->world );

        if( currentDistance <= 0.0001f )
        {
            return false;
        }

        if( m_pinchActive == false )
        {
            m_pinchActive = true;
            m_lastPinchDistance = currentDistance;

            return true;
        }

        const float difference = currentDistance - m_lastPinchDistance;
        m_lastPinchDistance = currentDistance;

        this->scale( 1.f + difference * m_wheelScaleFactor );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getScreenContentDelta_( const mt::vec2f & _screenDelta, const RenderContext * _context ) const
    {
        mt::vec2f contentDelta = _screenDelta;

        if( _context != nullptr && _context->resolution != nullptr )
        {
            _context->resolution->fromScreenToContentPosition( _screenDelta, &contentDelta );
        }

        return contentDelta;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getScreenWorldDelta_( const mt::vec2f & _screenDelta, const RenderContext * _context ) const
    {
        const mt::vec2f contentDelta = this->getScreenContentDelta_( _screenDelta, _context );

        Viewport viewport = m_viewport;

        if( m_renderViewport != nullptr )
        {
            viewport = m_renderViewport->getViewportWM();
        }

        const mt::vec2f viewportSize = viewport.size();
        const mt::vec2f boundsSize = m_bounds.size();

        mt::vec2f worldDelta( 0.f, 0.f );

        if( mt::abs_f( viewportSize.x ) > 0.0001f )
        {
            worldDelta.x = contentDelta.x * boundsSize.x / viewportSize.x;
        }

        if( mt::abs_f( viewportSize.y ) > 0.0001f )
        {
            worldDelta.y = -contentDelta.y * boundsSize.y / viewportSize.y;
        }

        return worldDelta;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getDragModeDelta_( const mt::vec2f & _delta ) const
    {
        if( m_draggingMode == EVADM_NONE )
        {
            return mt::vec2f( 0.f, 0.f );
        }

        mt::vec2f delta = _delta;

        if( m_draggingMode == EVADM_HORIZONTAL && m_scaleFactor >= 1.f )
        {
            delta.y = 0.f;
        }
        else if( m_draggingMode == EVADM_VERTICAL && m_scaleFactor >= 1.f )
        {
            delta.x = 0.f;
        }

        return delta;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getBoundsOffset_( const mt::vec2f & _position ) const
    {
        mt::vec2f offset( 0.f, 0.f );

        const float rightOffset = m_localBounds.begin.x - (_position.x + m_contentSize.x);
        const float leftOffset = m_localBounds.end.x - (_position.x + m_contentSize.z);
        const float topOffset = m_localBounds.begin.y - (_position.y + m_contentSize.y);
        const float bottomOffset = m_localBounds.end.y - (_position.y + m_contentSize.w);

        offset.x = rightOffset < 0.f ? rightOffset : leftOffset > 0.f ? leftOffset : 0.f;
        offset.y = topOffset < 0.f ? topOffset : bottomOffset > 0.f ? bottomOffset : 0.f;

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f VirtualArea::getModeDelta_( const mt::vec2f & _delta ) const
    {
        const float contentWidth = mt::abs_f( m_contentSize.z - m_contentSize.x );
        const float contentHeight = mt::abs_f( m_contentSize.w - m_contentSize.y );
        const bool invalidWidth = contentWidth < m_bounds.getWidth();
        const bool invalidHeight = contentHeight < m_bounds.getHeight();

        mt::vec2f delta = this->getDragModeDelta_( _delta );

        if( m_disableDragIfInvalid == true )
        {
            if( invalidWidth == true )
            {
                delta.x = 0.f;
            }

            if( invalidHeight == true )
            {
                delta.y = 0.f;
            }
        }

        return delta;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::clampPositionToBounds_( mt::vec2f * const _position ) const
    {
        mt::vec2f offset = this->getBoundsOffset_( *_position );

        if( this->isZero_( offset ) == true )
        {
            return false;
        }

        *_position += offset;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::isValidDrag_() const
    {
        if( m_disableDragIfInvalid == false )
        {
            return true;
        }

        const float contentWidth = mt::abs_f( m_contentSize.z - m_contentSize.x );
        const float contentHeight = mt::abs_f( m_contentSize.w - m_contentSize.y );

        return contentWidth >= m_bounds.getWidth() || contentHeight >= m_bounds.getHeight();
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::isZero_( const mt::vec2f & _value ) const
    {
        return mt::abs_f( _value.x ) < 0.0001f && mt::abs_f( _value.y ) < 0.0001f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::startElasticReturn_( const mt::vec2f & _offset )
    {
        if( this->isZero_( _offset ) == true )
        {
            return false;
        }

        const float distance = mt::length_v2( _offset );

        if( distance <= 0.0001f )
        {
            return false;
        }

        const mt::vec2f direction = _offset * (1.f / distance);
        const float velocityToBounds = mt::abs_f( m_velocity.x * direction.x + m_velocity.y * direction.y );
        const float minReturnSpeed = 0.35f + m_rigidity * 1.15f;
        const float returnSpeed = velocityToBounds < minReturnSpeed ? minReturnSpeed : velocityToBounds;

        const float duration = (3.f * distance) / returnSpeed;
        const float minDuration = 80.f;
        const float maxDuration = 750.f - m_rigidity * 350.f;

        m_returnStart = m_position;
        m_returnTarget = m_position + _offset;
        m_returnTime = 0.f;
        m_returnDuration = mt::clamp( minDuration, duration, maxDuration );
        m_returnActive = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualArea::updateElasticReturn_( float _dt )
    {
        if( m_returnActive == false )
        {
            return false;
        }

        m_returnTime += _dt;

        float t = m_returnDuration > 0.f ? mt::clamp( 0.f, m_returnTime / m_returnDuration, 1.f ) : 1.f;
        float inv = 1.f - t;
        float k = 1.f - inv * inv * inv;

        mt::vec2f delta = m_returnTarget - m_returnStart;
        mt::vec2f position = m_returnStart + delta * k;

        if( t >= 1.f )
        {
            position = m_returnTarget;
            m_velocity = mt::vec2f( 0.f, 0.f );
            m_returnActive = false;
        }
        else
        {
            float derivative = m_returnDuration > 0.f ? (3.f * inv * inv) / m_returnDuration : 0.f;
            m_velocity = delta * derivative;
        }

        this->setPositionInternal_( position, true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::stopElasticReturn_()
    {
        m_returnActive = false;
        m_returnTime = 0.f;
        m_returnDuration = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setPositionInternal_( const mt::vec2f & _position, bool _forceEvent )
    {
        if( _forceEvent == false && m_position == _position )
        {
            return;
        }

        m_position = _position;

        this->updateCameraPosition_();
        this->updateRenderScissor_();
        this->notifyDrag_();
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::notifyDrag_()
    {
        EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_DRAG )
            ->onVirtualAreaDrag( m_position, this->getPercentage() );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::notifyTouch_()
    {
        EVENTABLE_METHOD( EVENT_VIRTUAL_AREA_TOUCH )
            ->onVirtualAreaTouch( this->getTouchCount() );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualArea::setGlobalHandlerEnable_( bool _enable )
    {
        if( m_globalHandlerId == INVALID_UNIQUE_ID )
        {
            return;
        }

        const GlobalInputHandlerInterfacePtr & globalInputHandler = PLAYER_SERVICE()
            ->getGlobalInputHandler();

        globalInputHandler->enableGlobalHandler( m_globalHandlerId, _enable );
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualArea::TouchDesc * VirtualArea::getTouch_( ETouchCode _touchId )
    {
        const uint32_t index = (uint32_t)_touchId;

        if( index >= MENGINE_INPUT_MAX_TOUCH )
        {
            return nullptr;
        }

        return &m_touches[index];
    }
    //////////////////////////////////////////////////////////////////////////
    const VirtualArea::TouchDesc * VirtualArea::getTouch_( ETouchCode _touchId ) const
    {
        const uint32_t index = (uint32_t)_touchId;

        if( index >= MENGINE_INPUT_MAX_TOUCH )
        {
            return nullptr;
        }

        return &m_touches[index];
    }
    //////////////////////////////////////////////////////////////////////////
}
