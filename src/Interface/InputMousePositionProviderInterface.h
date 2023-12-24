#pragma once

#include "Interface/Interface.h"

#include "Kernel/TouchCode.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class InputMousePositionProviderInterface
        : public Interface
    {
    public:
        virtual void onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputMousePositionProviderInterface> InputMousePositionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
