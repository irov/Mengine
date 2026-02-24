#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleAdjustScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleAdjustScriptEmbedding );

    public:
        AppleAdjustScriptEmbedding();
        ~AppleAdjustScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
