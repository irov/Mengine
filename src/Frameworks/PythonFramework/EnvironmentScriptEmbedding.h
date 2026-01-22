#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class EnvironmentScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( EnvironmentScriptEmbedding );

    public:
        EnvironmentScriptEmbedding();
        ~EnvironmentScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}