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
    bool DummyPicker::pick( const mt::vec2f & _point, const RenderViewportInterfacePtr & _viewport, const RenderCameraInterfacePtr & _camera, const Resolution & _contentResolution, const ArrowPtr & _arrow ) const
    {
        MENGINE_UNUSED( _point );
        MENGINE_UNUSED( _viewport );
        MENGINE_UNUSED( _camera );
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
