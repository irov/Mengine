#pragma once

#include "PythonEventableProvider.h"

namespace Mengine
{
    class PythonEntityEventableProvider
        : public PythonEventableProviderInterface
    {
        DECLARE_FACTORABLE( PythonEntityEventableProvider );

    public:
        EventablePtr createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEntityEventableProvider, PythonEventableProviderInterface> PythonEntityEventableProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}