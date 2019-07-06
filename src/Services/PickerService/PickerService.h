#pragma once

#include "Interface/PickerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Viewport.h"

#include "Config/Vector.h"
#include "Config/List.h"

namespace Mengine
{
    struct PickerTrapStateDesc
    {
        PickerTrapState * state;

        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<PickerTrapStateDesc> VectorPickerTrapStates;
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
        bool pickTrap( const mt::vec2f& _point, VectorPickerTraps & _traps ) override;

    public:
        PickerTrapState * regTrap( const PickerTrapInterfacePtr & _trap, bool _exclusive ) override;
        void unregTrap( PickerTrapState * _id ) override;

        void updateTraps() override;
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
        void handleMouseEnter( const InputMousePositionEvent & _event ) override;
        void handleMouseLeave( const InputMousePositionEvent & _event ) override;

    public:
        uint32_t getPickerTrapCount() const override;

    private:
        bool proccesTraps_( float _x, float _y, VectorPickerTrapStates & _states );
        void updateDead_();

    protected:
        ArrowPtr m_arrow;
        ScenePtr m_scene;

        RenderViewportInterfacePtr m_viewport;
        RenderCameraInterfacePtr m_camera;
        RenderScissorInterfacePtr m_scissor;

        typedef List<PickerTrapState> ListPickerTrapState;
        ListPickerTrapState m_pickerTrapState;

        VectorPickerTrapStates m_states;

        uint32_t m_pickerTrapCount;
        uint32_t m_pickerTrapExclusiveCount;

        bool m_block;
        bool m_handleValue;

        bool m_invalidateTraps;
    };
}
