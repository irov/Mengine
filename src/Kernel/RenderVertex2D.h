#pragma once

#include "Config/Typedef.h"

#include "math/vec2.h"
#include "math/vec3.h"

#ifndef MENGINE_RENDER_VERTEX_UV_COUNT
#define MENGINE_RENDER_VERTEX_UV_COUNT 2
#endif

namespace Mengine
{
    struct RenderVertex2D
    {
        mt::vec3f position;
        uint32_t color;
        mt::vec2f uv[MENGINE_RENDER_VERTEX_UV_COUNT];
    };
}