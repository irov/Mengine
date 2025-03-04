#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AppleAdvertisementScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AppleAdvertisementScriptEmbedding();
        ~AppleAdvertisementScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
