#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class JSONScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( JSONScriptEmbedding );

    public:
        JSONScriptEmbedding();
        ~JSONScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}