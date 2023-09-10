#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceInternalObject
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceInternalObject();
        ~LoaderResourceInternalObject() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}