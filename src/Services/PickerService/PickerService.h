#pragma once

#include "Interface/PickerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Viewport.h"
#include "Kernel/Vector.h"
#include "Kernel/RenderContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct PickerStateDesc
    {
        PickerInterface * picker;

        RenderContext context;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<PickerStateDesc> VectorPickerStates;
    //////////////////////////////////////////////////////////////////////////
    class PickerService
        : public ServiceBase<PickerServiceInterface>
    {
    public:
        PickerService();
        ~PickerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setBlock( bool _value ) override;
        void setHandleValue( bool _value ) override;

    public:
        void setArrow( const ArrowPtr & _arrow ) override;
        void setScene( const ScenePtr & _scene ) override;

    public:
        void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) override;
        void setRenderCamera( const RenderCameraInterfacePtr & _camera ) override;
        void setRenderTransformation( const RenderTransformationInterfacePtr & _transformation ) override;
        void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) override;
        void setRenderTarget( const RenderTargetInterfacePtr & _target ) override;

    public:
        void update() override;

    public:
        bool pickTraps( const mt::vec2f & _point, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickers * const _pickers ) const override;
        bool getTraps( const mt::vec2f & _point, VectorPickers * const _pickers ) const override;

    public:
        void invalidateTraps() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        void updateTraps_();

    protected:
        void fillStates_( VectorPickerStates * const _states ) const;
        bool pickStates_( float _x, float _y, ETouchCode _touchId, float _pressure, const InputSpecialData & _special, VectorPickerStates * const _states ) const;
        bool getStates_( float _x, float _y, VectorPickerStates * const _states ) const;

    protected:
        ArrowPtr m_arrow;
        ScenePtr m_scene;

        RenderViewportInterfacePtr m_viewport;
        RenderCameraInterfacePtr m_camera;
        RenderTransformationInterfacePtr m_transformation;
        RenderScissorInterfacePtr m_scissor;
        RenderTargetInterfacePtr m_target;

        VectorPickerStates m_states;

        bool m_block;
        bool m_handleValue;

        bool m_invalidateTraps;
    };
}
