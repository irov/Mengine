#pragma once

#include "Kernel/BasePicker.h"

namespace Mengine
{
    class DummyPicker
        : public BasePicker
    {
    public:
        DummyPicker();
        ~DummyPicker() override;

    public:
        MENGINE_INLINE bool isPickerDummy() const override;

    public:
        bool pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const override;

    protected:
        Scriptable * getPickerScriptable() override;
        Eventable * getPickerEventable() override;
        InputHandlerInterface * getPickerInputHandler() override;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool DummyPicker::isPickerDummy() const
    {
        return true;
    }
}