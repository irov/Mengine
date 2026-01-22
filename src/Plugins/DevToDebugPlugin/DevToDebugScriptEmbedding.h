#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class DevToDebugScriptEmbedding
        : public ScriptEmbeddingInterface
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