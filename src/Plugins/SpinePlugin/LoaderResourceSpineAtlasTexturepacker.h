#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineAtlasTexturepacker
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineAtlasTexturepacker();
        ~LoaderResourceSpineAtlasTexturepacker() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
