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
    bool DummyPicker::pick( const mt::vec2f & _point, const RenderContext * _context, const Resolution & _contentResolution, const ArrowInterfacePtr & _arrow ) const
    {
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _contentResolution );
        MENGINE_UNUSED( _arrow );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Scriptable * DummyPicker::getPickerScriptable()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    Eventable * DummyPicker::getPickerEventable()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    InputHandlerInterface * DummyPicker::getPickerInputHandler()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
