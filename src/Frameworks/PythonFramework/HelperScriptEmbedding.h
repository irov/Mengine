#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class HelperScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( HelperScriptEmbedding );

    public:
        HelperScriptEmbedding();
        ~HelperScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}