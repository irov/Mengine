#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleMARSDKScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleMARSDKScriptEmbedding );

    public:
        AppleMARSDKScriptEmbedding();
        ~AppleMARSDKScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}