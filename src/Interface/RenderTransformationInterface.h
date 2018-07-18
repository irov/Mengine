#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTransformationInterface
        : public Mixin
    {
    public:
        virtual const mt::mat4f & getTransformationWorldMatrix() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTransformationInterface> RenderTransformationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}