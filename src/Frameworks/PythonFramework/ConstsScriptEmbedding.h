#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class ConstsScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( ConstsScriptEmbedding );

    public:
        ConstsScriptEmbedding();
        ~ConstsScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}