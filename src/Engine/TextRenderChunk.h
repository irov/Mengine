#pragma once

#include "Interface/RenderMaterialInterface.h"

namespace Mengine
{
    struct TextRenderChunk
    {
        uint32_t vertex_begin;
        uint32_t vertex_count;

        RenderMaterialInterfacePtr material;
    };
}