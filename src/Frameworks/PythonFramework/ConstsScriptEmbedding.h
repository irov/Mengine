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
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}