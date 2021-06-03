#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#define MENGINE_RENDER_ZGROUP_DEFAULT 0x7fffffff
#define MENGINE_RENDER_ZINDEX_DEFAULT 0x7fffffff

namespace Mengine
{
    typedef int32_t ZGroupType;
    typedef int32_t ZIndexType;

    struct RenderContext
    {
        const RenderViewportInterface * viewport;
        const RenderCameraInterface * camera;
        const RenderTransformationInterface * transformation;
        const RenderScissorInterface * scissor;
        const RenderTargetInterface * target;

        ZGroupType zGroup;
        ZIndexType zIndex;
    };
}