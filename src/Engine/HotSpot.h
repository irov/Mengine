#pragma once

#include "Kernel/Node.h"

#include "HotSpotEventReceiverInterface.h"

#include "Kernel/Polygon.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BasePicker.h"

namespace Mengine
{
    class HotSpot
        : public Node
        , public BaseEventation
        , public BasePicker
        , public InputHandlerInterface
    {
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE( HotSpotEventReceiverInterface );
        DECLARE_PICKER();

    public:
        HotSpot();
        ~HotSpot() override;

    public:
        void setOutward( bool _value );
        bool getOutward() const;

    public:
        void setGlobal( bool _value );
        bool getGlobal() const;

    public:
        void setExclusive( bool _value );
        bool getExclusive() const;

    public:
        void setDefaultHandle( bool _handle );
        bool getDefaultHandle() const;

    public:
        bool isMousePickerOver() const;

    public:
        virtual bool testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const = 0;
        virtual bool testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const = 0;
        virtual bool testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const = 0;

    protected:
        bool _activate() override;
        void _afterActivate() override;
        void _deactivate() override;

    protected:
        void _freeze( bool _value ) override;

    protected:
        bool pick( const mt::vec2f& _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const override;

    public:
        Scriptable * getPickerScriptable() override;
        InputHandlerInterface * getPickerInputHandler() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        void activatePicker_();
        void deactivatePicker_();

    protected:
        bool m_outward;
        bool m_global;
        bool m_exclusive;

        bool m_defaultHandle;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpot, Node> HotSpotPtr;
    //////////////////////////////////////////////////////////////////////////
}
