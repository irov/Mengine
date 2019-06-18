#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct TextRenderChunk
    {
        uint32_t vertex_begin;
        uint32_t vertex_count;

        RenderMaterialInterfacePtr material;
    };
}