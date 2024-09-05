#pragma once

#include "Interface/Interface.h"
#include "Interface/PickerInputHandlerInterface.h"

#include "Kernel/Resolution.h"
#include "Kernel/BoundingBox.h"
#include "Kernel/RenderContext.h"
#include "Kernel/Eventable.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/Scriptable.h"
#endif

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Pickerable;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ArrowInterface> ArrowInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PickerInterface
        : public Interface
    {
    public:
        virtual Pickerable * getPickerable() = 0;

    public:
        virtual void setRelationPicker( PickerInterface * _relationPicker ) = 0;
        virtual void setRelationPickerFront( PickerInterface * _relationPicker ) = 0;
        virtual void removeRelationPicker() = 0;
        virtual PickerInterface * getRelationPicker() const = 0;

    public:
        virtual bool isEmptyPickerChildren() const = 0;
        virtual void clearPickerChildren() = 0;

    public:
        typedef Lambda<void( PickerInterface * )> LambdaPicker;
        virtual void foreachPickerChildren( const LambdaPicker & _lambda ) = 0;
        virtual void foreachPickerChildrenEnabled( const LambdaPicker & _lambda ) = 0;

    public:
        virtual void updatePickers() = 0;

    public:
        virtual void setPickerEnable( bool _enable ) = 0;
        virtual bool isPickerEnable() const = 0;

    public:
        virtual bool isPickerDummy() const = 0;

    public:
        virtual void setPickerPicked( bool _picked ) = 0;
        virtual bool isPickerPicked() const = 0;
        virtual void setPickerPressed( bool _picked ) = 0;
        virtual bool isPickerPressed() const = 0;
        virtual void setPickerHandle( bool _picked ) = 0;
        virtual bool isPickerHandle() const = 0;
        virtual void setPickerExclusive( bool _picked ) = 0;
        virtual bool isPickerExclusive() const = 0;

#if defined(MENGINE_USE_SCRIPT_SERVICE)
    public:
        virtual Scriptable * getPickerScriptable() = 0;
#endif

    public:
        virtual Eventable * getPickerEventable() = 0;

        virtual PickerInputHandlerInterface * getPickerInputHandler() = 0;

    public:
        virtual const RenderViewportInterfacePtr & getPickerViewport() const = 0;
        virtual const RenderCameraInterfacePtr & getPickerCamera() const = 0;
        virtual const RenderScissorInterfacePtr & getPickerScissor() const = 0;
        virtual const RenderTransformationInterfacePtr & getPickerTransformation() const = 0;
        virtual const RenderTargetInterfacePtr & getPickerTarget() const = 0;

    public:
        virtual bool pick( const mt::vec2f & _point, const RenderContext * _context, const Resolution & _contentResolution, const ArrowInterfacePtr & _arrow ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickerInterface> PickerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}