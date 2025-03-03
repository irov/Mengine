#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class GameScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( GameScriptEmbedding );

    public:
        GameScriptEmbedding();
        ~GameScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}