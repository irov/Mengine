#pragma once

#include "UIControlCallbackInterface.h"
#include "UIProgressBar.h"

#include "Kernel/Node.h"
#include "Kernel/HotSpot.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UISlider
        : public Node
        , public PickerInputHandlerInterface
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( UISlider );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        UISlider();
        ~UISlider() override;

    public:
        void setHotSpot( const HotSpotPtr & _hotSpot );
        const HotSpotPtr & getHotSpot() const;

        void setThumbNode( const NodePtr & _thumbNode );
        const NodePtr & getThumbNode() const;

        void setProgressBar( const UIProgressBarPtr & _progressBar );
        const UIProgressBarPtr & getProgressBar() const;

        void setTrackOffset( const mt::vec2f & _offset );
        const mt::vec2f & getTrackOffset() const;

        void setTrackSize( const mt::vec2f & _size );
        const mt::vec2f & getTrackSize() const;

        void setDirection( EUIProgressBarDirection _direction );
        EUIProgressBarDirection getDirection() const;

        void setRange( float _minimum, float _maximum );
        float getMinimum() const;
        float getMaximum() const;

        void setStep( float _step );
        float getStep() const;

        void setValue( float _value );
        float getValue() const;
        float getProgress() const;

        void setChangeCallback( const UIFloatValueCallbackInterfacePtr & _callback );
        const UIFloatValueCallbackInterfacePtr & getChangeCallback() const;

    protected:
        void _dispose() override;

    public:
        bool handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event ) override;
        bool handleTextEvent( const RenderContext * _context, const InputTextEvent & _event ) override;
        bool handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) override;
        bool handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override;

    private:
        float normalizeValue_( float _value ) const;
        void setValueFromWorldPoint_( const mt::vec2f & _worldPoint );
        void updateVisuals_();

    private:
        HotSpotPtr m_hotSpot;
        NodePtr m_thumbNode;
        UIProgressBarPtr m_progressBar;
        UIFloatValueCallbackInterfacePtr m_changeCallback;

        mt::vec2f m_trackOffset;
        mt::vec2f m_trackSize;

        float m_minimum;
        float m_maximum;
        float m_step;
        float m_value;

        EUIProgressBarDirection m_direction;
        ETouchCode m_dragTouchId;
        bool m_dragging;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UISlider> UISliderPtr;
    //////////////////////////////////////////////////////////////////////////
}
