#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "AreaOfInterestInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class AreaOfInterestScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AreaOfInterestScriptEmbedding );

    public:
        AreaOfInterestScriptEmbedding();
        ~AreaOfInterestScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}