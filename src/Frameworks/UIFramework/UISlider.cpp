#include "UISlider.h"

#include "Interface/TransformationInterface.h"

#include "math/mat4.h"

#include <cmath>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UISlider::UISlider()
        : m_trackOffset( 0.f, 0.f )
        , m_trackSize( 100.f, 20.f )
        , m_minimum( 0.f )
        , m_maximum( 1.f )
        , m_step( 0.f )
        , m_value( 0.f )
        , m_direction( EUIPBD_LEFT_TO_RIGHT )
        , m_dragTouchId( TC_TOUCH0 )
        , m_dragging( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UISlider::~UISlider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setHotSpot( const HotSpotPtr & _hotSpot )
    {
        if( m_hotSpot == _hotSpot )
        {
            return;
        }

        PickerInputHandlerInterfacePtr handler = PickerInputHandlerInterfacePtr::from( this );

        if( m_hotSpot != nullptr )
        {
            m_hotSpot->removePickerInputHandler( handler );

            if( m_hotSpot->getParent() == this )
            {
                m_hotSpot->removeFromParent();
            }
        }

        m_hotSpot = _hotSpot;

        if( m_hotSpot != nullptr )
        {
            m_hotSpot->setDefaultHandle( false );
            m_hotSpot->addPickerInputHandler( handler, MENGINE_DOCUMENT_FACTORABLE );
            this->addChild( m_hotSpot );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const HotSpotPtr & UISlider::getHotSpot() const
    {
        return m_hotSpot;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setThumbNode( const NodePtr & _thumbNode )
    {
        if( m_thumbNode == _thumbNode )
        {
            return;
        }

        if( m_thumbNode != nullptr && m_thumbNode->getParent() == this )
        {
            m_thumbNode->removeFromParent();
        }

        m_thumbNode = _thumbNode;

        if( m_thumbNode != nullptr )
        {
            this->addChild( m_thumbNode );
        }

        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & UISlider::getThumbNode() const
    {
        return m_thumbNode;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setProgressBar( const UIProgressBarPtr & _progressBar )
    {
        if( m_progressBar == _progressBar )
        {
            return;
        }

        if( m_progressBar != nullptr && m_progressBar->getParent() == this )
        {
            m_progressBar->removeFromParent();
        }

        m_progressBar = _progressBar;

        if( m_progressBar != nullptr )
        {
            this->addChild( m_progressBar );
        }

        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    const UIProgressBarPtr & UISlider::getProgressBar() const
    {
        return m_progressBar;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setTrackOffset( const mt::vec2f & _offset )
    {
        if( m_trackOffset == _offset )
        {
            return;
        }

        m_trackOffset = _offset;

        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UISlider::getTrackOffset() const
    {
        return m_trackOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setTrackSize( const mt::vec2f & _size )
    {
        mt::vec2f size(
            _size.x > 0.f ? _size.x : 0.f,
            _size.y > 0.f ? _size.y : 0.f );

        if( m_trackSize == size )
        {
            return;
        }

        m_trackSize = size;

        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & UISlider::getTrackSize() const
    {
        return m_trackSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setDirection( EUIProgressBarDirection _direction )
    {
        if( m_direction == _direction )
        {
            return;
        }

        m_direction = _direction;

        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    EUIProgressBarDirection UISlider::getDirection() const
    {
        return m_direction;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setRange( float _minimum, float _maximum )
    {
        if( _minimum <= _maximum )
        {
            m_minimum = _minimum;
            m_maximum = _maximum;
        }
        else
        {
            m_minimum = _maximum;
            m_maximum = _minimum;
        }

        this->setValue( m_value );
        this->updateVisuals_();
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::getMinimum() const
    {
        return m_minimum;
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::getMaximum() const
    {
        return m_maximum;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setStep( float _step )
    {
        float step = _step > 0.f ? _step : 0.f;

        if( m_step == step )
        {
            return;
        }

        m_step = step;

        this->setValue( m_value );
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::getStep() const
    {
        return m_step;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setValue( float _value )
    {
        float value = this->normalizeValue_( _value );

        if( m_value == value )
        {
            return;
        }

        m_value = value;

        this->updateVisuals_();

        if( m_changeCallback != nullptr )
        {
            m_changeCallback->onUIFloatValue( m_value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::getProgress() const
    {
        float range = m_maximum - m_minimum;

        if( range <= 0.f )
        {
            return 0.f;
        }

        return (m_value - m_minimum) / range;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setChangeCallback( const UIFloatValueCallbackInterfacePtr & _callback )
    {
        m_changeCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    const UIFloatValueCallbackInterfacePtr & UISlider::getChangeCallback() const
    {
        return m_changeCallback;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::_dispose()
    {
        if( m_hotSpot != nullptr )
        {
            PickerInputHandlerInterfacePtr handler = PickerInputHandlerInterfacePtr::from( this );
            m_hotSpot->removePickerInputHandler( handler );
        }

        m_dragging = false;
        m_changeCallback = nullptr;
        m_progressBar = nullptr;
        m_thumbNode = nullptr;
        m_hotSpot = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleTextEvent( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        if( _event.button != MC_LBUTTON || _event.isDown == false )
        {
            return false;
        }

        m_dragging = true;
        m_dragTouchId = _event.touchId;

        this->setValueFromWorldPoint_( _event.position.world );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );

        if( m_dragging == false || _event.touchId != m_dragTouchId || _event.button != MC_LBUTTON || _event.isDown == true )
        {
            return false;
        }

        this->setValueFromWorldPoint_( _event.position.world );

        m_dragging = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        MENGINE_UNUSED( _context );

        if( m_dragging == false || _event.touchId != m_dragTouchId )
        {
            return false;
        }

        this->setValueFromWorldPoint_( _event.position.world );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UISlider::handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    float UISlider::normalizeValue_( float _value ) const
    {
        float value = _value;

        if( value < m_minimum )
        {
            value = m_minimum;
        }
        else if( value > m_maximum )
        {
            value = m_maximum;
        }

        if( m_step > 0.f )
        {
            value = m_minimum + std::round( (value - m_minimum) / m_step ) * m_step;

            if( value > m_maximum )
            {
                value = m_maximum;
            }
        }

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::setValueFromWorldPoint_( const mt::vec2f & _worldPoint )
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mat4f wmInv;
        mt::inv_m4_m4( &wmInv, wm );

        mt::vec2f localPoint;
        mt::mul_v2_v2_m4( &localPoint, _worldPoint, wmInv );

        float progress;

        switch( m_direction )
        {
        case EUIPBD_LEFT_TO_RIGHT:
            progress = m_trackSize.x > 0.f ? (localPoint.x - m_trackOffset.x) / m_trackSize.x : 0.f;
            break;
        case EUIPBD_RIGHT_TO_LEFT:
            progress = m_trackSize.x > 0.f ? 1.f - (localPoint.x - m_trackOffset.x) / m_trackSize.x : 0.f;
            break;
        case EUIPBD_TOP_TO_BOTTOM:
            progress = m_trackSize.y > 0.f ? (localPoint.y - m_trackOffset.y) / m_trackSize.y : 0.f;
            break;
        case EUIPBD_BOTTOM_TO_TOP:
            progress = m_trackSize.y > 0.f ? 1.f - (localPoint.y - m_trackOffset.y) / m_trackSize.y : 0.f;
            break;
        default:
            progress = 0.f;
            break;
        }

        this->setValue( m_minimum + (m_maximum - m_minimum) * progress );
    }
    //////////////////////////////////////////////////////////////////////////
    void UISlider::updateVisuals_()
    {
        float progress = this->getProgress();

        if( m_progressBar != nullptr )
        {
            m_progressBar->setDirection( m_direction );
            m_progressBar->setProgress( progress );
        }

        if( m_thumbNode == nullptr )
        {
            return;
        }

        TransformationInterface * transformation = m_thumbNode->getTransformation();

        if( transformation == nullptr )
        {
            return;
        }

        mt::vec2f position = m_trackOffset;

        switch( m_direction )
        {
        case EUIPBD_LEFT_TO_RIGHT:
            position.x += m_trackSize.x * progress;
            position.y += m_trackSize.y * 0.5f;
            break;
        case EUIPBD_RIGHT_TO_LEFT:
            position.x += m_trackSize.x * (1.f - progress);
            position.y += m_trackSize.y * 0.5f;
            break;
        case EUIPBD_TOP_TO_BOTTOM:
            position.x += m_trackSize.x * 0.5f;
            position.y += m_trackSize.y * progress;
            break;
        case EUIPBD_BOTTOM_TO_TOP:
            position.x += m_trackSize.x * 0.5f;
            position.y += m_trackSize.y * (1.f - progress);
            break;
        default:
            break;
        }

        const mt::vec3f & currentPosition = transformation->getLocalPosition();
        transformation->setLocalPosition( mt::vec3f( position.x, position.y, currentPosition.z ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
