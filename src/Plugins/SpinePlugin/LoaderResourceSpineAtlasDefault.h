#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class LoaderResourceSpineAtlasDefault
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        LoaderResourceSpineAtlasDefault();
        ~LoaderResourceSpineAtlasDefault() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
