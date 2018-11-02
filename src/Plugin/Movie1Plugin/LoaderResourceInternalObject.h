#pragma once

#include "Interface/LoaderServiceInterface.h"

namespace Mengine
{
    class LoaderResourceInternalObject
        : public LoaderInterface
    {
    public:
        LoaderResourceInternalObject();
        ~LoaderResourceInternalObject() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}