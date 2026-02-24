#pragma once

#include "Interface/ServantInterface.h"

#include "Environment/Python/PythonForwardDeclaration.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScriptEmbeddingInterface
        : public ServantInterface
    {
    public:
        virtual bool embed( pybind::kernel_interface * _kernel ) = 0;
        virtual void eject( pybind::kernel_interface * _kernel ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScriptEmbeddingInterface> ScriptEmbeddingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}