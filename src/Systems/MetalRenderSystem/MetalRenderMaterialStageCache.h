#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetalRenderMaterialStageCache
        : public RenderMaterialStageCacheInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderMaterialStageCache );

    public:
        MetalRenderMaterialStageCache();
        ~MetalRenderMaterialStageCache() override;
    };

    typedef IntrusivePtr<MetalRenderMaterialStageCache, RenderMaterialStageCacheInterface> MetalRenderMaterialStageCachePtr;
}

