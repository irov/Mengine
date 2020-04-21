#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "AreaOfInterestInterface.h"

#include "Kernel/Factorable.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class AreaOfInterestScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        AreaOfInterestScriptEmbedding();
        ~AreaOfInterestScriptEmbedding() override;

    public:
        bool embedding( pybind::kernel_interface * _kernel ) override;
        void ejecting( pybind::kernel_interface * _kernel ) override;
    };
}