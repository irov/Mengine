#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "PathfinderInterface.h"

namespace Mengine
{
    class PathfinderScriptEmbedding
        : public ScriptEmbeddingInterface
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