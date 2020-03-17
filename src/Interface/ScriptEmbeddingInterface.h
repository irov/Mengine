#pragma once

#include "Interface/Interface.h"

namespace pybind
{
    class kernel_interface;
}

namespace Mengine
{
    class ScriptEmbeddingInterface
        : public Interface
    {
    public:
        virtual bool embedding( pybind::kernel_interface * _kernel ) = 0;
        virtual void ejecting( pybind::kernel_interface * _kernel ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddingInterface> ScriptEmbeddingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}