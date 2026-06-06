#pragma once

#include "BatchRenderPipeline.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class Forward3DRenderPipeline
        : public BatchRenderPipeline
    {
        DECLARE_FACTORABLE( Forward3DRenderPipeline );

    public:
        Forward3DRenderPipeline();
        ~Forward3DRenderPipeline() override;

    public:
        void render() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
