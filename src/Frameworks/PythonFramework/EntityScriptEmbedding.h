#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class EntityScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( EntityScriptEmbedding );

    public:
        EntityScriptEmbedding();
        ~EntityScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}