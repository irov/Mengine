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
        MENGINE_INLINE bool isPickerDummy() const override;

    public:
        bool pick( const mt::vec2f & _point, const RenderContext * _context ) const override;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    protected:
        Scriptable * getPickerScriptable() override;
#endif

    protected:
        Eventable * getPickerEventable() override;
        PickerInputHandlerInterface * getPickerInputHandler() override;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool DummyPicker::isPickerDummy() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}