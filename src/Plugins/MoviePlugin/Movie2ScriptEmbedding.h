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
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}