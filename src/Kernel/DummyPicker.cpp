#include "Kernel/DummyPicker.h"

#include "Interface/PickerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummyPicker::DummyPicker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummyPicker::~DummyPicker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyPicker::pick( const mt::vec2f & _point, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _context );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    //////////////////////////////////////////////////////////////////////////
    Scriptable * DummyPicker::getPickerScriptable()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    Eventable * DummyPicker::getPickerEventable()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PickerInputHandlerInterface * DummyPicker::getPickerInputHandler()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
