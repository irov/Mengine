#pragma once

#include "PythonEventableProvider.h"

namespace Mengine
{
    class PythonSceneEventableProvider
        : public PythonEventableProviderInterface
    {
        DECLARE_FACTORABLE( PythonSceneEventableProvider );

    public:
        EventablePtr createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonSceneEventableProvider, PythonEventableProviderInterface> PythonSceneEventableProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}