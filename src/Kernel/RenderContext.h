#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

namespace Mengine
{
    struct RenderContext
    {
        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
        RenderTransformationInterfacePtr transformation;
        RenderScissorInterfacePtr scissor;
        RenderTargetInterfacePtr target;
    };
}