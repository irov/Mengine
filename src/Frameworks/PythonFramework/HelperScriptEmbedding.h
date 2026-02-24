#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class HelperScriptEmbedding
        : public ScriptEmbeddingInterface
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