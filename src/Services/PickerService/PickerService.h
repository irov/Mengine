#pragma once

#include "Interface/PickerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Viewport.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    struct PickerStateDesc
    {
        PickerInterface * picker;

        const RenderViewportInterface * viewport;
        const RenderCameraInterface * camera;
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
        void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) override;

    public:
        void update() override;
        void clear() override;

    public:
        bool pickTraps( const mt::vec2f & _point, uint32_t _touchId, float _pressure, VectorPickers * const _pickers ) override;
        bool getTraps( const mt::vec2f & _point, VectorPickers * const _pickers ) override;

    public:
        void invalidateTraps() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

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
        bool pickStates_( float _x, float _y, uint32_t _touchId, float _pressure, VectorPickerStates * const _states );
        bool getStates_( float _x, float _y, VectorPickerStates * const _states );

    protected:
        ArrowPtr m_arrow;
        ScenePtr m_scene;

        RenderViewportInterfacePtr m_viewport;
        RenderCameraInterfacePtr m_camera;
        RenderScissorInterfacePtr m_scissor;

        VectorPickerStates m_states;

        bool m_block;
        bool m_handleValue;

        bool m_invalidateTraps;
    };
}
