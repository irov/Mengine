#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleStoreInAppPurchaseScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleStoreInAppPurchaseScriptEmbedding );

    public:
        AppleStoreInAppPurchaseScriptEmbedding();
        ~AppleStoreInAppPurchaseScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
