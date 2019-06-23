#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Resolution.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scriptable;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Arrow, class Node> ArrowPtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class PickerTrapInterface> PickerTrapInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct PickerTrapState
    {
        PickerTrapInterfacePtr trap;

        uint32_t id;
        bool picked;
        bool pressed;
        bool handle;
        bool dead;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderViewportInterface> RenderViewportInterfacePtr;
    typedef IntrusivePtr<class RenderCameraInterface> RenderCameraInterfacePtr;
    typedef IntrusivePtr<class RenderScissorInterface> RenderScissorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PickerTrapInterface
        : public InputHandlerInterface
    {
    public:
        virtual bool pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) = 0;

    public:
        virtual bool onHandleMouseEnter( float _x, float _y ) = 0;
        virtual void onHandleMouseLeave() = 0;
        virtual void onHandleMouseOverDestroy() = 0;

    public:
        virtual PickerTrapState * propagatePickerTrapState() const = 0;
        virtual Scriptable * propagatePickerScriptable() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickerTrapInterface> PickerTrapInterfacePtr;
    //////////////////////////////////////////////////////////////////////////   
}