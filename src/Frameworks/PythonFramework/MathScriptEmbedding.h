#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class MathScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( MathScriptEmbedding );

    public:
        MathScriptEmbedding();
        ~MathScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}