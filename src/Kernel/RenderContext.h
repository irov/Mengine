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
        RenderViewportInterface * viewport;
        RenderCameraInterface * camera;
        RenderTransformationInterface * transformation;
        RenderScissorInterface * scissor;
        RenderTargetInterface * target;
    };
}