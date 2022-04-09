#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "DevToDebugInterface.h"

#include "Kernel/Params.h"
#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class DevToDebugScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        DevToDebugScriptEmbedding();
        ~DevToDebugScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;
    };
}