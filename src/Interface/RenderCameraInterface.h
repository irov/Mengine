#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraInterface
        : public Mixin
    {
    public:
        virtual const mt::mat4f & getCameraViewMatrix() const = 0;
        virtual const mt::mat4f & getCameraViewMatrixInv() const = 0;

        virtual const mt::mat4f & getCameraProjectionMatrix() const = 0;
        virtual const mt::mat4f & getCameraProjectionMatrixInv() const = 0;

    public:
        virtual const mt::mat4f & getCameraViewProjectionMatrix() const = 0;
        virtual const mt::mat4f & getCameraViewProjectionMatrixInv() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraInterface> RenderCameraInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}