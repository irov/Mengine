#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class iOSUserMessagingPlatformScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( iOSUserMessagingPlatformScriptEmbedding );

    public:
        iOSUserMessagingPlatformScriptEmbedding();
        ~iOSUserMessagingPlatformScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
