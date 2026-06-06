#pragma once

#include "Config/Typedef.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

#ifndef MENGINE_RENDER_VERTEX3D_UV_COUNT
#define MENGINE_RENDER_VERTEX3D_UV_COUNT 2
#endif

namespace Mengine
{
    struct RenderVertex3D
    {
        mt::vec3f position;
        mt::vec3f normal;
        mt::vec4f tangent;
        uint32_t color;
        mt::vec2f uv[MENGINE_RENDER_VERTEX3D_UV_COUNT];
        uint32_t _padding;
    };
}
