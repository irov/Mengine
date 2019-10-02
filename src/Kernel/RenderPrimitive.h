#pragma once

namespace Mengine
{
    struct RenderPrimitive
    {
        RenderMaterialInterface * material;

        uint32_t baseVertexIndex;
        uint32_t minIndex;
        uint32_t vertexCount;
        uint32_t startIndex;
        uint32_t indexCount;
    };
}