#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class GraphicsScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        GraphicsScriptEmbedding();
        ~GraphicsScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}