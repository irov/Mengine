#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MovieScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        MovieScriptEmbedding();
        ~MovieScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting() override;
    };
}