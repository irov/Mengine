#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TiledMapScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( TiledMapScriptEmbedding );

    public:
        TiledMapScriptEmbedding();
        ~TiledMapScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
