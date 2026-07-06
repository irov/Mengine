#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceSpineAtlasTexturepacker
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceSpineAtlasTexturepacker();
        ~MetabufLoaderResourceSpineAtlasTexturepacker() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
