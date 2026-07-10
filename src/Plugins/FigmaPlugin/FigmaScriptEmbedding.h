#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class FigmaScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( FigmaScriptEmbedding );

    public:
        FigmaScriptEmbedding();
        ~FigmaScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}
