#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleStoreReviewScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AppleStoreReviewScriptEmbedding();
        ~AppleStoreReviewScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
