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
    bool DummyPicker::pick( const mt::vec2f & _point, const RenderContext * _context, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const
    {
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _arrow );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
