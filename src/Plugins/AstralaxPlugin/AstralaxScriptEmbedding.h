#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AstralaxScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AstralaxScriptEmbedding );

    public:
        AstralaxScriptEmbedding();
        ~AstralaxScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}