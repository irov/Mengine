#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class SteamScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( SteamScriptEmbedding );

    public:
        SteamScriptEmbedding();
        ~SteamScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}