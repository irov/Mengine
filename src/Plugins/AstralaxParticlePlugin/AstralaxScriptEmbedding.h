#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AstralaxScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AstralaxScriptEmbedding();
        ~AstralaxScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}