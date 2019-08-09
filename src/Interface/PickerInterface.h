#pragma once

#include "Interface/Interface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/Resolution.h"
#include "Kernel/BoundingBox.h"
#include "Kernel/RenderContext.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Eventable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Arrow, class Node> ArrowPtr;
    //////////////////////////////////////////////////////////////////////////
    class PickerInterface
        : public Interface
    {
    public:
        virtual void setRelationPickerBack( PickerInterface * _relationPicker ) = 0;
        virtual void setRelationPickerFront( PickerInterface * _relationPicker ) = 0;
        virtual void removeRelationPicker() = 0;
        virtual PickerInterface * getRelationPicker() const = 0;

    public:
        virtual void moveRelationPickerFront( PickerInterface * _childPicker ) = 0;
        virtual void moveRelationPickerMiddle( PickerInterface * _afterPicker, PickerInterface * _childPicker ) = 0;
        virtual void moveRelationPickerBack( PickerInterface * _childPicker ) = 0;

    public:
        virtual bool emptyPickerChildren() const = 0;

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

    public:
        virtual Scriptable * getPickerScriptable() = 0;
        virtual Eventable * getPickerEventable() = 0;

        virtual InputHandlerInterface * getPickerInputHandler() = 0;


    public:
        virtual const RenderViewportInterfacePtr & getPickerViewport() const = 0;
        virtual const RenderCameraInterfacePtr & getPickerCamera() const = 0;

    public:
        virtual bool pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PickerInterface> PickerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}