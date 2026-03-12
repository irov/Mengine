#pragma once

#include "PythonEventableProvider.h"

namespace Mengine
{
    class PythonLayerEventableProvider
        : public PythonEventableProviderInterface
    {
        DECLARE_FACTORABLE( PythonLayerEventableProvider );

    public:
        EventablePtr createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayerEventableProvider, PythonEventableProviderInterface> PythonLayerEventableProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}