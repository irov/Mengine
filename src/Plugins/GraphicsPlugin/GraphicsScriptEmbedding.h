#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class GraphicsScriptEmbedding
        : public ScriptEmbeddingInterface
    {
    public:
        GraphicsScriptEmbedding();
        ~GraphicsScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}