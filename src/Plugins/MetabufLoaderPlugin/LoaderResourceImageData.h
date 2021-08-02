#pragma once

#include "Interface/LoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceImageData
        : public LoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceImageData();
        ~LoaderResourceImageData() override;

    protected:
        bool load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta ) override;
    };
}