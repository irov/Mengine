#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "PathfinderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PathfinderScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PathfinderScriptEmbedding );

    public:
        PathfinderScriptEmbedding();
        ~PathfinderScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}