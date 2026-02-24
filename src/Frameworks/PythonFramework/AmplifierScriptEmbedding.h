#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

namespace Mengine
{
    class AmplifierScriptEmbedding
        : public ScriptEmbeddingInterface
    {
        DECLARE_FACTORABLE( AmplifierScriptEmbedding );

    public:
        AmplifierScriptEmbedding();
        ~AmplifierScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        FactorablePtr m_implement;
    };
}