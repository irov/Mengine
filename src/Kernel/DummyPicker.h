#pragma once

#include "Kernel/BasePicker.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DummyPicker
        : public BasePicker
    {
    public:
        DummyPicker();
        ~DummyPicker() override;

    public:
        bool pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const override;

    protected:
        Scriptable * getPickerScriptable() override;
        InputHandlerInterface * getPickerInputHandler() override;
    };
}