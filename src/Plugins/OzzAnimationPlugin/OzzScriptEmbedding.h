#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class OzzScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( OzzScriptEmbedding );

    public:
        OzzScriptEmbedding();
        ~OzzScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}