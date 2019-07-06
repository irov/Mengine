#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/PickerTrapInterface.h"

#include "Config/Vector.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Scene, class Node> ScenePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<PickerTrapInterfacePtr> VectorPickerTraps;
    //////////////////////////////////////////////////////////////////////////
    class PickerServiceInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "PickerService" );

    public:
        virtual void setBlock( bool _value ) = 0;
        virtual void setHandleValue( bool _value ) = 0;

    public:
        virtual void setArrow( const ArrowPtr & _arrow ) = 0;
        virtual void setScene( const ScenePtr & _scene ) = 0;

    public:
        virtual void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) = 0;
        virtual void setRenderCamera( const RenderCameraInterfacePtr & _camera ) = 0;
        virtual void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) = 0;

    public:
        virtual void update() = 0;
        virtual void clear() = 0;

    public:
        virtual bool pickTrap( const mt::vec2f& _point, VectorPickerTraps & _traps ) = 0;

    public:
        virtual PickerTrapState * regTrap( const PickerTrapInterfacePtr & _trap, bool _exclusive ) = 0;
        virtual void unregTrap( PickerTrapState * _id ) = 0;

        virtual void updateTraps() = 0;
        virtual void invalidateTraps() = 0;

    public:
        virtual uint32_t getPickerTrapCount() const = 0;

    public:
        virtual void handleMouseEnter( const InputMousePositionEvent & _event ) = 0;
        virtual void handleMouseLeave( const InputMousePositionEvent & _event ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PICKER_SERVICE()\
    ((Mengine::PickerServiceInterface *)SERVICE_GET(Mengine::PickerServiceInterface))
//////////////////////////////////////////////////////////////////////////