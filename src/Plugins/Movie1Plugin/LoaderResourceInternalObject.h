#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceInternalObject
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceInternalObject();
        ~LoaderResourceInternalObject() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}