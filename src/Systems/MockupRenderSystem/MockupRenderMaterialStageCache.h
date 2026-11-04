#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderMaterialStageCache
        : public RenderMaterialStageCacheInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderMaterialStageCache );

    public:
        MockupRenderMaterialStageCache();
        ~MockupRenderMaterialStageCache() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderMaterialStageCache, RenderMaterialStageCacheInterface> MockupRenderMaterialStageCachePtr;
    //////////////////////////////////////////////////////////////////////////
}
