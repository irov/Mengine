#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "PathfinderInterface.h"

#include "Kernel/Factorable.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class PathfinderScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        PathfinderScriptEmbedding();
        ~PathfinderScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}