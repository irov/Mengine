#pragma once

#include "Interface/RenderOrderInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

namespace Mengine
{
    struct RenderContext
    {
        const RenderOrderInterface * order;
        const RenderViewportInterface * viewport;
        const RenderCameraInterface * camera;
        const RenderTransformationInterface * transformation;
        const RenderScissorInterface * scissor;
        const RenderTargetInterface * target;
    };
}