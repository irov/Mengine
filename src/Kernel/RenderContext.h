#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#define MENGINE_RENDER_ZGROUP_DEFAULT 0x7f
#define MENGINE_RENDER_ZINDEX_DEFAULT 0x7fffffff

namespace Mengine
{
    struct RenderContext
    {
        const RenderViewportInterface * viewport;
        const RenderCameraInterface * camera;
        const RenderTransformationInterface * transformation;
        const RenderScissorInterface * scissor;
        const RenderTargetInterface * target;

        int8_t zGroup;
        int32_t zIndex;
    };
}