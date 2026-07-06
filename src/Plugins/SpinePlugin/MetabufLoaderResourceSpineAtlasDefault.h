#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceSpineAtlasDefault
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceSpineAtlasDefault();
        ~MetabufLoaderResourceSpineAtlasDefault() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
