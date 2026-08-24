#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzleScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( DazzleScriptEmbedding );

    public:
        DazzleScriptEmbedding();
        ~DazzleScriptEmbedding() override;

        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
