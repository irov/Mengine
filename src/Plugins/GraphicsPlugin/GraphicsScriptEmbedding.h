#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class GraphicsScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( GraphicsScriptEmbedding );

    public:
        GraphicsScriptEmbedding();
        ~GraphicsScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}