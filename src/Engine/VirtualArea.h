#pragma once

#include "Engine/VirtualAreaEventReceiverInterface.h"

#include "Interface/PickerInputHandlerInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BasePicker.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Engine/HotSpotPolygon.h"
#include "Kernel/Viewport.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaGlobalInputHandler;
    //////////////////////////////////////////////////////////////////////////
    enum EVirtualAreaDragMode
    {
        EVADM_NONE = 0,
        EVADM_FREE,
        EVADM_HORIZONTAL,
        EVADM_VERTICAL,
    };
    //////////////////////////////////////////////////////////////////////////
    class VirtualArea
        : public Node
        , public PickerInputHandlerInterface
        , protected BaseRender
        , protected BasePicker
        , protected BaseEventation
        , protected BaseTransformation
        , protected BaseUpdation
    {
        friend class VirtualAreaGlobalInputHandler;

        DECLARE_FACTORABLE( VirtualArea );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();
        DECLARE_UPDATABLE();

    public:
        VirtualArea();
        ~VirtualArea() override;

    public:
        void setViewport( const Viewport & _viewport );
        const Viewport & getViewport() const;
        bool hasViewport() const;
        void setViewportFromHotSpot( const HotSpotPolygonPtr & _hotSpot );

    public:
        void setContentSize( float _left, float _top, float _right, float _bottom );
        const mt::vec4f & getContentSize() const;

    public:
        void addContentNode( const NodePtr & _node, bool _useAsAnchor );

    public:
        void setPosition( const mt::vec2f & _position );
        const mt::vec2f & getPosition() const;

        void setAnchor( const mt::vec2f & _anchor );
        const mt::vec2f & getAnchor() const;

        void setPercentage( const mt::vec2f & _percentage );
        void setPercentageX( float _percentage );
        void setPercentageY( float _percentage );
        mt::vec2f getPercentage() const;

    public:
        void scale( float _factor );
        void setScale( float _scaleFactor );
        float getScaleFactor() const;

        void setMaxScaleFactor( float _maxScaleFactor );
        float getMaxScaleFactor() const;

        void setScaleEnable( bool _enable );
        bool getScaleEnable() const;

        void setWheelScaleFactor( float _factor );
        float getWheelScaleFactor() const;

    public:
        void setDraggingMode( EVirtualAreaDragMode _mode );
        EVirtualAreaDragMode getDraggingMode() const;
        void setDraggingModeName( const ConstString & _mode );
        const ConstString & getDraggingModeName() const;

        void setFriction( float _friction );
        float getFriction() const;

        void setRigidity( float _rigidity );
        float getRigidity() const;

        void setAllowOutOfBounds( bool _allow );
        bool getAllowOutOfBounds() const;

        void setDisableDragIfInvalid( bool _disable );
        bool getDisableDragIfInvalid() const;

        void setDragStartThreshold( float _threshold );
        float getDragStartThreshold() const;

        void setDefaultHandle( bool _handle );
        bool getDefaultHandle() const;

        void freeze( bool _value );
        bool isFrozen() const;

    public:
        const mt::vec2f & getVelocity() const;
        bool isDragging() const;
        uint32_t getTouchCount() const;
        mt::vec2f getViewportSize() const;
        mt::vec2f getContentSizeValue() const;

    public:
        const RenderCameraOrthogonalPtr & getRenderCameraNode() const;
        const RenderViewportPtr & getRenderViewportNode() const;
        const RenderScissorPtr & getRenderScissorNode() const;

    protected:
        bool _activate() override;
        void _deactivate() override;
        void _dispose() override;
        void _freeze( bool _value ) override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool pick( const mt::vec2f & _point, const RenderContext * _context ) const override;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    protected:
        Scriptable * getPickerScriptable() override;
#endif

    protected:
        Eventable * getPickerEventable() override;
        PickerInputHandlerInterface * getPickerInputHandler() override;

    protected:
        bool handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event ) override;
        bool handleTextEvent( const RenderContext * _context, const InputTextEvent & _event ) override;
        bool handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) override;
        bool handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override;

    protected:
        const RenderResolutionInterfacePtr & getPickerResolution() const override;
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;
        const RenderTransformationInterfacePtr & getPickerTransformation() const override;
        const RenderTargetInterfacePtr & getPickerTarget() const override;

    protected:
        void ensureRenderContextNodes_();
        void updateRenderContextNodes_();
        void updateCameraPosition_();
        void updateRenderScissor_();
        void updateScaleViewport_();
        void updateLocalBounds_();
        void validatePosition_();
        void stopDrag_();

    protected:
        bool beginTouch_( const RenderContext * _context, const InputMouseButtonEvent & _event );
        bool moveTouch_( const InputMouseMoveEvent & _event );
        bool endTouch_( const InputMouseButtonEvent & _event );
        bool releaseTouch_( ETouchCode _touchId );
        bool updatePinch_();

    protected:
        mt::vec2f getScreenContentDelta_( const mt::vec2f & _screenDelta, const RenderContext * _context ) const;
        mt::vec2f getScreenWorldDelta_( const mt::vec2f & _screenDelta, const RenderContext * _context ) const;
        mt::vec2f getDragModeDelta_( const mt::vec2f & _delta ) const;
        mt::vec2f getBoundsOffset_( const mt::vec2f & _position ) const;
        mt::vec2f getModeDelta_( const mt::vec2f & _delta ) const;
        bool clampPositionToBounds_( mt::vec2f * const _position ) const;
        bool isValidDrag_() const;
        bool isZero_( const mt::vec2f & _value ) const;
        bool startElasticReturn_( const mt::vec2f & _offset );
        bool updateElasticReturn_( float _dt );
        void stopElasticReturn_();
        void setPositionInternal_( const mt::vec2f & _position, bool _forceEvent );
        void notifyDrag_();
        void notifyTouch_();
        void setGlobalHandlerEnable_( bool _enable );

    protected:
        struct TouchDesc
        {
            bool active;
            bool pressed;
            bool dragging;
            ETouchCode touchId;
            RenderContext context;
            mt::vec2f startScreen;
            mt::vec2f screen;
            mt::vec2f previousScreen;
            mt::vec2f startWorld;
            mt::vec2f world;
            mt::vec2f previousWorld;
        };

        TouchDesc * getTouch_( ETouchCode _touchId );
        const TouchDesc * getTouch_( ETouchCode _touchId ) const;

    protected:
        Viewport m_viewport;
        Viewport m_bounds;
        Viewport m_localBounds;
        mt::vec4f m_contentSize;

        mt::vec2f m_anchor;
        mt::vec2f m_position;
        mt::vec2f m_velocity;
        mt::vec2f m_returnStart;
        mt::vec2f m_returnTarget;

        float m_scaleFactor;
        float m_maxScaleFactor;
        float m_wheelScaleFactor;
        float m_friction;
        float m_rigidity;
        float m_dragStartThreshold;
        float m_lastPinchDistance;
        float m_returnTime;
        float m_returnDuration;

        EVirtualAreaDragMode m_draggingMode;
        ConstString m_draggingModeName;

        RenderCameraOrthogonalPtr m_renderCamera;
        RenderViewportPtr m_renderViewport;
        RenderScissorPtr m_renderScissor;

        Vector<TouchDesc> m_touches;

        InputHandlerInterfacePtr m_globalHandler;
        UniqueId m_globalHandlerId;

        bool m_hasViewport;
        bool m_enableScale;
        bool m_allowOutOfBounds;
        bool m_disableDragIfInvalid;
        bool m_defaultHandle;
        bool m_frozen;
        bool m_dragging;
        bool m_pinchActive;
        bool m_returnActive;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<VirtualArea> VirtualAreaPtr;
    //////////////////////////////////////////////////////////////////////////
}
