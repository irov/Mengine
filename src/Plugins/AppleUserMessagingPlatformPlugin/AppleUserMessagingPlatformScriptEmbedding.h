#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleUserMessagingPlatformScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleUserMessagingPlatformScriptEmbedding );
        
    public:
        AppleUserMessagingPlatformScriptEmbedding();
        ~AppleUserMessagingPlatformScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
