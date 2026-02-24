#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AppleFacebookScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AppleFacebookScriptEmbedding );

    public:
        AppleFacebookScriptEmbedding();
        ~AppleFacebookScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
