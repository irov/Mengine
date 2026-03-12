#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Eventable.h"

namespace Mengine
{
    class PythonEventableProviderInterface
        : public Factorable
    {
    public:
        ~PythonEventableProviderInterface() override = default;

    public:
        virtual EventablePtr createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEventableProviderInterface> PythonEventableProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}