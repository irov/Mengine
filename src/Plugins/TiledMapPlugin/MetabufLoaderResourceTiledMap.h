#pragma once

#include "Interface/MetabufLoaderInterface.h"

#include "ResourceTiledMap.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetabufLoaderResourceTiledMap
        : public MetabufLoaderInterface
        , public Factorable
    {
    public:
        MetabufLoaderResourceTiledMap();
        ~MetabufLoaderResourceTiledMap() override;

    protected:
        bool load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta ) override;
    };
}
