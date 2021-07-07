#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class OzzScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        OzzScriptEmbedding();
        ~OzzScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}