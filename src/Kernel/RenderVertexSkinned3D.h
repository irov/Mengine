#pragma once

#include "Config/Typedef.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

#ifndef MENGINE_RENDER_VERTEX_SKINNED3D_UV_COUNT
#define MENGINE_RENDER_VERTEX_SKINNED3D_UV_COUNT 2
#endif

#ifndef MENGINE_RENDER_VERTEX_SKINNED3D_INFLUENCES
#define MENGINE_RENDER_VERTEX_SKINNED3D_INFLUENCES 4
#endif

#ifndef MENGINE_RENDER_BONE_MAX
#define MENGINE_RENDER_BONE_MAX 128
#endif

namespace Mengine
{
    struct RenderVertexSkinned3D
    {
        mt::vec3f position;
        mt::vec3f normal;
        mt::vec4f tangent;
        uint32_t color;
        mt::vec2f uv[MENGINE_RENDER_VERTEX_SKINNED3D_UV_COUNT];
        uint8_t boneIndices[MENGINE_RENDER_VERTEX_SKINNED3D_INFLUENCES];
        float boneWeights[MENGINE_RENDER_VERTEX_SKINNED3D_INFLUENCES];
    };
}
