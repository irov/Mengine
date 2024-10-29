#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Params.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class EnvironmentScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        EnvironmentScriptEmbedding();
        ~EnvironmentScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}