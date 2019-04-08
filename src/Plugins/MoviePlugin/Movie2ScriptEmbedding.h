#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class Movie2ScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        Movie2ScriptEmbedding();
        ~Movie2ScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}