#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class ConstsScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        ConstsScriptEmbedding();
        ~ConstsScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}