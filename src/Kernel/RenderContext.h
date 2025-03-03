#pragma once

#include "Interface/RenderResolutionInterface.h"
#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#ifndef MENGINE_RENDER_ZGROUP_DEFAULT
#define MENGINE_RENDER_ZGROUP_DEFAULT 0x7fffffff
#endif

#ifndef MENGINE_RENDER_ZINDEX_DEFAULT
#define MENGINE_RENDER_ZINDEX_DEFAULT 0x7fffffff
#endif

namespace Mengine
{
    typedef int32_t ZGroupType;
    typedef int32_t ZIndexType;

    struct RenderContext
    {
        const RenderResolutionInterface * resolution;
        const RenderViewportInterface * viewport;
        const RenderCameraInterface * camera;
        const RenderTransformationInterface * transformation;
        const RenderScissorInterface * scissor;
        const RenderTargetInterface * target;

        ZGroupType zGroup;
        ZIndexType zIndex;
    };
}