#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class MCPScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( MCPScriptEmbedding );

    public:
        MCPScriptEmbedding();
        ~MCPScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
