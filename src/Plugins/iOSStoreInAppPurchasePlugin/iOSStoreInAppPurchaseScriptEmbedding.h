#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSStoreInAppPurchaseScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSStoreInAppPurchaseScriptEmbedding );

    public:
        iOSStoreInAppPurchaseScriptEmbedding();
        ~iOSStoreInAppPurchaseScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
