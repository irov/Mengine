#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "DevToDebugInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DevToDebugScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DevToDebugScriptEmbedding );

    public:
        DevToDebugScriptEmbedding();
        ~DevToDebugScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}